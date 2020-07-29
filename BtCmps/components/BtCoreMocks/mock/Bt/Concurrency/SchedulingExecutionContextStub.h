//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_SchedulingExecutionContextStub__h
#define INC__Bt_Concurrency_SchedulingExecutionContextStub__h

/*
inline std::ostream& operator<<(std::ostream& pOut, const std::chrono::milliseconds& pDuration) {
   pOut << pDuration.count() << "ms";
   return pOut;
}
*/

#include <trompeloeil.hpp>

#include "Bt/Concurrency/I_SchedulingExecutionContext.h"

namespace Bt {
namespace Concurrency {

class SchedulingExecutionContextStub : public I_SchedulingExecutionContext {
   public:

      SchedulingExecutionContextStub(){
         mPrevious = setCurrentExecutionContext(this);   
      }

      ~SchedulingExecutionContextStub(){
         setCurrentExecutionContext(mPrevious);   
      }

      virtual PeriodicTimer callPeriodically(const std::chrono::milliseconds& pPeriod, std::function<void(I_PeriodicTimer&)> pFunction) override {
         return std::make_shared<PeriodicTimerImplStub>();
      }

      virtual OnceTimer callOnce(const std::chrono::milliseconds& pDelay, std::function<void()> pFunction) override {
         return std::make_shared<OnceTimerImplStub>();   
      }

      virtual void call(std::function<void()> pFunction) override {
      }

   private:
      class PeriodicTimerImplStub : public I_PeriodicTimer {
         virtual void cancel() override {};
      };

      class OnceTimerImplStub : public I_OnceTimer {
         virtual void cancel() override {};
      };

      I_ExecutionContext* mPrevious;
};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_SchedulingExecutionContextStub__h

