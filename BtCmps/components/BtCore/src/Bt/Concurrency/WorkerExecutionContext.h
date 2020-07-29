//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::WorkerExecutionContext
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_WorkerExecutionContext__h
#define INC__Bt_Concurrency_WorkerExecutionContext__h

#include <thread>
#include <atomic>

#include "Bt/Concurrency/I_ExecutionContext.h"
#include "Bt/Concurrency/CountingLatch.h"
#include "Bt/Concurrency/BlockingQueue.h"

namespace Bt {
namespace Concurrency {

class WorkerExecutionContext : public I_ExecutionContext
{
   public:
      
      WorkerExecutionContext();
      WorkerExecutionContext(const WorkerExecutionContext&) = delete;
      WorkerExecutionContext& operator=(const WorkerExecutionContext&) = delete;
      virtual ~WorkerExecutionContext();

      virtual void call(std::function<void()> iFunction) override;

      void run();
      
      void stop() {
         stopInternal();
         mCountingLatch.wait();
      }

      template< class Rep, class Period>
      bool stop(const std::chrono::duration<Rep, Period>& pTimeout) {
         stopInternal();
         return mCountingLatch.waitFor(pTimeout);
      } 

   private:
      void workcycle();
      void stopInternal();

      CountingLatch mCountingLatch;
      BlockingQueue<std::function<void()>> mQueue;
      std::atomic<bool> mRunning;
};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_WorkerExecutionContext__h

