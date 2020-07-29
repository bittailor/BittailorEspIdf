//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Concurrency::I_SchedulingExecutionContext
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_I_SchedulingExecutionContext__h
#define INC__Bt_Concurrency_I_SchedulingExecutionContext__h

#include "Bt/Concurrency/I_ExecutionContext.h"
#include "Bt/Concurrency/I_PeriodicTimer.h"
#include "Bt/Concurrency/I_OnceTimer.h"


namespace Bt {
namespace Concurrency {

class I_SchedulingExecutionContext : public I_ExecutionContext {
   public:
      typedef std::shared_ptr<I_PeriodicTimer> PeriodicTimer;
      typedef std::shared_ptr<I_OnceTimer> OnceTimer;

      virtual ~I_SchedulingExecutionContext() {}
      
      virtual PeriodicTimer callPeriodically(const std::chrono::milliseconds& pPeriod, std::function<void(I_PeriodicTimer&)> pFunction) = 0;
      virtual OnceTimer callOnce(const std::chrono::milliseconds& pDelay, std::function<void()> pFunction) = 0;
}; 

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_I_SchedulingExecutionContext__h

