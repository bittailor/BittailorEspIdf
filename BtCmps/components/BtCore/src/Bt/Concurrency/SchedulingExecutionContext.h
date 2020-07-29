//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::SchedulingExecutionContext
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_SchedulingExecutionContext__h
#define INC__Bt_Concurrency_SchedulingExecutionContext__h

#include <thread>

#include <Bt/Concurrency/SchedulingExecutionContextBase.h>
#include <Bt/Concurrency/BlockingQueue.h>

namespace Bt {
namespace Concurrency {

class SchedulingExecutionContext : public SchedulingExecutionContextBase
{
   public:

      SchedulingExecutionContext(Core::I_Time& pTime);
      SchedulingExecutionContext(const SchedulingExecutionContext&) = delete;
      SchedulingExecutionContext& operator=(const SchedulingExecutionContext&) = delete;
      ~SchedulingExecutionContext();

      void run();
      void stop();

   protected:
      virtual std::chrono::milliseconds maxWorkcycleTimeout();

   private:
      bool mRunning;
};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_SchedulingExecutionContext__h
