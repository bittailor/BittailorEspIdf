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
         ESP_LOGW(TAG, "Missed %d periods of %ums\n", missedPeriods ,mPeriod);
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
   uint32_t now = mTime.milliseconds();
   auto timer = std::make_shared<PeriodicTimerImpl>(now, pPeriod.count(), pFunction);
   insertTimer(timer, now);
   mQueue.push([](){});
   return timer;
}

SchedulingExecutionContextBase::OnceTimer SchedulingExecutionContextBase::callOnce(const std::chrono::milliseconds& pDelay, std::function<void()> pFunction) {
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
   std::chrono::milliseconds delay = std::chrono::milliseconds(processTimers());
   size_t numberOfQueuedCalls =  mQueue.size();
   for (size_t i = 0; i < numberOfQueuedCalls; i++)
   {
      std::function<void ()> function;
      mQueue.pop(function);
      function();
   }
   std::chrono::milliseconds timeout = std::min(delay, maxWorkcycleTimeout());
   mQueue.waitFor(timeout);
}

void SchedulingExecutionContextBase::insertTimer(std::shared_ptr<I_ExecutionContextTimer> pTimer, uint32_t pNow) {
   mTimers.insert(std::lower_bound(begin(mTimers), end(mTimers), pTimer, [pNow](const std::shared_ptr<I_ExecutionContextTimer>& element, const std::shared_ptr<I_ExecutionContextTimer>& value){
      return element->millisecondsToTarget(pNow) < value->millisecondsToTarget(pNow);	
   }), pTimer);
}

uint32_t SchedulingExecutionContextBase::processTimers() {
   uint32_t now = mTime.milliseconds();
   while (!mTimers.empty()) {
      auto timer = mTimers.front();
      uint32_t delay = timer->millisecondsToTarget(now);
      if (delay > 0) {
         return delay;
      }
      mTimers.pop_front();
      bool reschedule = timer->execute(now);
      if(reschedule) {
         insertTimer(timer, mTime.milliseconds());
      }
   }
   return std::numeric_limits<uint32_t>::max();	
}

} // namespace Concurrency
} // namespace Bt

