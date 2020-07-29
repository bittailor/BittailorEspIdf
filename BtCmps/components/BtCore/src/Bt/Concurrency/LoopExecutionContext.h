//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::LoopExecutionContext
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_LoopExecutionContext__h
#define INC__Bt_Concurrency_LoopExecutionContext__h

#include <thread>

#include <Bt/Concurrency/SchedulingExecutionContextBase.h>
#include <Bt/Concurrency/BlockingQueue.h>

namespace Bt {
namespace Concurrency {

class LoopExecutionContext : public SchedulingExecutionContextBase
{
   public:

      LoopExecutionContext(Core::I_Time& pTime, std::chrono::milliseconds pMaxLoopTimeout);
      LoopExecutionContext(const LoopExecutionContext&) = delete;
      LoopExecutionContext& operator=(const LoopExecutionContext&) = delete;
      ~LoopExecutionContext();

      void loop();
   protected:
      
      virtual std::chrono::milliseconds maxWorkcycleTimeout() override;

   private:
      std::chrono::milliseconds mMaxLoopTimeout;  

};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_LoopExecutionContext__h
