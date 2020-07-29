//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::SchedulingExecutionContextBase
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_SchedulingExecutionContextBase__h
#define INC__Bt_Concurrency_SchedulingExecutionContextBase__h

#include <chrono>
#include <memory>
#include <list>

#include <Bt/Core/I_Time.h>
#include <Bt/Concurrency/I_SchedulingExecutionContext.h>
#include <Bt/Concurrency/BlockingQueue.h>

namespace Bt {
namespace Concurrency {

class SchedulingExecutionContextBase : public I_SchedulingExecutionContext
{
   public:

      SchedulingExecutionContextBase(Core::I_Time& pTime);
      SchedulingExecutionContextBase(const SchedulingExecutionContextBase&) = delete;
      SchedulingExecutionContextBase& operator=(const SchedulingExecutionContextBase&) = delete;
      ~SchedulingExecutionContextBase();

      virtual PeriodicTimer callPeriodically(const std::chrono::milliseconds& pPeriod, std::function<void(I_PeriodicTimer&)> pFunction) override;
      virtual OnceTimer callOnce(const std::chrono::milliseconds& pDelay, std::function<void()> pFunction) override;

      virtual void call(std::function<void()> pFunction) override;

   protected:
      void workcycle();
      virtual std::chrono::milliseconds maxWorkcycleTimeout() = 0;

   private:
      class I_ExecutionContextTimer {
         public:
            virtual ~I_ExecutionContextTimer() {}

            virtual uint32_t millisecondsToTarget(uint32_t millisecondsTimestamp) = 0;
            virtual bool execute(uint32_t millisecondsTimestamp) = 0;
      };

      class PeriodicTimerImpl;
      class OnceTimerImpl;


      void insertTimer(std::shared_ptr<I_ExecutionContextTimer> pTimer, uint32_t pNow); 
      uint32_t processTimers();

      typedef std::list<std::shared_ptr<I_ExecutionContextTimer>> Timers;
      
      Core::I_Time& mTime;
      Timers mTimers;
      BlockingQueue<std::function<void()>> mQueue;

      
};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_SchedulingExecutionContextBase__h

