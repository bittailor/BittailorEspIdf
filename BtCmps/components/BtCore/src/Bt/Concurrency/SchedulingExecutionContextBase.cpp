//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::SchedulingExecutionContextBase
//
//*************************************************************************************************

#include "Bt/Concurrency/SchedulingExecutionContextBase.h"

#include <limits>
#include <algorithm>

#include "Bt/Concurrency/Tag.h"

namespace Bt {
namespace Concurrency {

// *** PeriodicTimerImpl

   class SchedulingExecutionContextBase::PeriodicTimerImpl : public SchedulingExecutionContextBase::I_ExecutionContextTimer , public I_PeriodicTimer {
      public:
         PeriodicTimerImpl(uint32_t pStartTime , uint32_t pPeriod, std::function<void(I_PeriodicTimer&)> pFunction);
         virtual ~PeriodicTimerImpl();

         virtual uint32_t millisecondsToTarget(uint32_t pMillisecondsTimestamp);
         virtual bool execute(uint32_t pMillisecondsTimestamp);
         virtual void cancel();
      private:
         uint32_t mStartTime;
         const uint32_t mPeriod;
         std::function<void(I_PeriodicTimer&)> mFunction;
         bool mCanceled;
   };

   SchedulingExecutionContextBase::PeriodicTimerImpl::PeriodicTimerImpl(uint32_t pStartTime , uint32_t pPeriod, std::function<void(I_PeriodicTimer&)> pFunction)
   : mStartTime(pStartTime), mPeriod(pPeriod), mFunction(pFunction), mCanceled(false)   {
   }

   SchedulingExecutionContextBase::PeriodicTimerImpl::~PeriodicTimerImpl() {

   }

   uint32_t SchedulingExecutionContextBase::PeriodicTimerImpl::millisecondsToTarget(uint32_t pMillisecondsTimestamp) {
      uint32_t diff = pMillisecondsTimestamp - mStartTime;
      if (diff >= mPeriod) {
         return 0;
      }
      return mPeriod - diff;
   }

   bool SchedulingExecutionContextBase::PeriodicTimerImpl::execute(uint32_t pMillisecondsTimestamp) {
      if(mCanceled) { 
         return false; 
      }
      mFunction(*this);
      mStartTime = mStartTime + mPeriod;
      int missedPeriods = 0;
      while(millisecondsToTarget(pMillisecondsTimestamp) == 0){
         missedPeriods++;
         mStartTime = mStartTime + mPeriod;
      }
      if(missedPeriods > 0) {
         ESP_LOGW(TAG, "Missed %d periods of %ums", missedPeriods ,mPeriod);
      }
      return !mCanceled;
   }

   void SchedulingExecutionContextBase::PeriodicTimerImpl::cancel() {
      mCanceled = true;
   }


// *** OnceTimerImpl

   class SchedulingExecutionContextBase::OnceTimerImpl : public SchedulingExecutionContextBase::I_ExecutionContextTimer , public I_OnceTimer {
      public:
         OnceTimerImpl(uint32_t pStartTime , uint32_t pDelay, std::function<void()> pFunction);
         virtual ~OnceTimerImpl();

         virtual uint32_t millisecondsToTarget(uint32_t pMillisecondsTimestamp);
         virtual bool execute(uint32_t pMillisecondsTimestamp);
         virtual void cancel();
      private:
         uint32_t mStartTime;
         const uint32_t mDelay;
         std::function<void()> mFunction;
         bool mCanceled;
   };

   SchedulingExecutionContextBase::OnceTimerImpl::OnceTimerImpl(uint32_t pStartTime , uint32_t pDelay, std::function<void()> pFunction)
   : mStartTime(pStartTime), mDelay(pDelay), mFunction(pFunction), mCanceled(false)   {
   }

   SchedulingExecutionContextBase::OnceTimerImpl::~OnceTimerImpl() {

   }

   uint32_t SchedulingExecutionContextBase::OnceTimerImpl::millisecondsToTarget(uint32_t pMillisecondsTimestamp) {
      uint32_t diff = pMillisecondsTimestamp - mStartTime;
      if (diff >= mDelay) {
         return 0;
      }
      return mDelay - diff;
   }

   bool SchedulingExecutionContextBase::OnceTimerImpl::execute(uint32_t pMillisecondsTimestamp) {
      if(!mCanceled) { 
         mFunction();
      }
      return false;
   }

   void SchedulingExecutionContextBase::OnceTimerImpl::cancel() {
      mCanceled = true;
   }

// *** 

SchedulingExecutionContextBase::SchedulingExecutionContextBase(Core::I_Time& pTime) : mTime(pTime) {

}

SchedulingExecutionContextBase::~SchedulingExecutionContextBase() {
}

SchedulingExecutionContextBase::PeriodicTimer SchedulingExecutionContextBase::callPeriodically(const std::chrono::milliseconds& pPeriod, std::function<void(I_PeriodicTimer&)> pFunction) { 
   ESP_LOGV(TAG, " -> callPeriodically %d", (int)pPeriod.count());
   uint32_t now = mTime.milliseconds();
   auto timer = std::make_shared<PeriodicTimerImpl>(now, pPeriod.count(), pFunction);
   insertTimer(timer, now);
   mQueue.push([](){});
   return timer;
}

SchedulingExecutionContextBase::OnceTimer SchedulingExecutionContextBase::callOnce(const std::chrono::milliseconds& pDelay, std::function<void()> pFunction) {
   ESP_LOGV(TAG, " -> callOnce %d", (int)pDelay.count());
   uint32_t now = mTime.milliseconds();
   auto timer = std::make_shared<OnceTimerImpl>(now, pDelay.count(), pFunction);
   insertTimer(timer, now);
   mQueue.push([](){});
   return timer;
}


void SchedulingExecutionContextBase::call(std::function<void()> pFunction) {
   mQueue.push(std::move(pFunction));
}

void SchedulingExecutionContextBase::workcycle() {
   ESP_LOGV(TAG, " -> workcycle enter");
   auto t1 = mTime.microseconds();
   std::chrono::milliseconds delay = std::chrono::milliseconds(processTimers());
   auto t2 = mTime.microseconds();
   size_t numberOfQueuedCalls =  mQueue.size();
   for (size_t i = 0; i < numberOfQueuedCalls; i++)
   {
      std::function<void ()> function;
      mQueue.pop(function);
      function();
   }
   auto t3 = mTime.microseconds();
   auto tTimers = t2-t1;
   auto tCalls = t3-t2;
   ESP_LOGV(TAG, " -> workcycle t => tTimers = %d us tCalls = %d us", tTimers, tCalls);
   std::chrono::milliseconds timeout = std::min(delay, maxWorkcycleTimeout());
   ESP_LOGV(TAG, " -> workcycle exit wait => timeout = %d delay = %d maxWorkcycleTimeout = %d", (int)timeout.count(), (int)delay.count(), (int)maxWorkcycleTimeout().count());
   mQueue.waitFor(timeout);
}

void SchedulingExecutionContextBase::insertTimer(std::shared_ptr<I_ExecutionContextTimer> pTimer, uint32_t pNow) {
   mTimers.insert(std::lower_bound(begin(mTimers), end(mTimers), pTimer, [pNow](const std::shared_ptr<I_ExecutionContextTimer>& element, const std::shared_ptr<I_ExecutionContextTimer>& value){
      return element->millisecondsToTarget(pNow) < value->millisecondsToTarget(pNow);	
   }), pTimer);
}

uint32_t SchedulingExecutionContextBase::processTimers() {
   ESP_LOGV(TAG, " -> processTimers enter mTimers.size() = %d", mTimers.size());
   while (!mTimers.empty()) {
      auto timer = mTimers.front();
      uint32_t delay = timer->millisecondsToTarget(mTime.milliseconds());
      if (delay > 0) {
         ESP_LOGV(TAG, " -> processTimers exit mTimers.size() = %d delay = %d", mTimers.size(), delay);
         return delay;
      }
      mTimers.pop_front();
      ESP_LOGV(TAG, " -> processTimers execute start");
      bool reschedule = timer->execute(mTime.milliseconds());
      ESP_LOGV(TAG, " -> processTimers execute end");
      if(reschedule) {
         ESP_LOGV(TAG, " -> processTimers reschedule");
         insertTimer(timer, mTime.milliseconds());
      }
   }
   ESP_LOGV(TAG, " -> processTimers exit mTimers.size() = %d delay = max", mTimers.size());
   return std::numeric_limits<uint32_t>::max();	
}

} // namespace Concurrency
} // namespace Bt

