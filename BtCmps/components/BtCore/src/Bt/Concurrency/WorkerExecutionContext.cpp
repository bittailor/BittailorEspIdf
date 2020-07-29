//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::WorkerExecutionContext
//
//*************************************************************************************************

#include "Bt/Concurrency/WorkerExecutionContext.h"

#include <Bt/Core/Logging.h>

#include "Bt/Concurrency/Tag.h"

namespace Bt {
namespace Concurrency {

//-------------------------------------------------------------------------------------------------

WorkerExecutionContext::WorkerExecutionContext() 
: mCountingLatch(0), mRunning(true) {
}

//-------------------------------------------------------------------------------------------------

WorkerExecutionContext::~WorkerExecutionContext() {
   if(mRunning) {
      if(!stop(std::chrono::seconds(10))) {
         ESP_LOGE(TAG, "failed stoping in destructor");
      }
   }
}

//-------------------------------------------------------------------------------------------------

void WorkerExecutionContext::call(std::function<void()> iFunction) {
   mQueue.push(std::move(iFunction));
}

//-------------------------------------------------------------------------------------------------

void WorkerExecutionContext::run() {
   auto previous = setCurrentExecutionContext(this);
   mCountingLatch.countUp();
   while(mRunning) {
      workcycle();
   }

   std::function<void ()> function;
   while(mQueue.tryPop(function)) {
      function();
   }
   setCurrentExecutionContext(previous);
   mCountingLatch.countDown();
}

//-------------------------------------------------------------------------------------------------

void WorkerExecutionContext::workcycle() {
   std::function<void ()> function;
   mQueue.pop(function);
   try {
      function();
   } catch(const std::exception& exception) {
      ESP_LOGE(TAG, "WorkerExecutionContext caught a std exception: %s", exception.what());
   } catch (...) {
      ESP_LOGE(TAG, "WorkerExecutionContext caught a non-std exception!");
   }
}

//-------------------------------------------------------------------------------------------------

void WorkerExecutionContext::stopInternal() {
   auto count = mCountingLatch.count();
   mRunning = false;
   for(size_t i = 0 ; i < count ; i++) {
      mQueue.push([this](){});
   }
}

//-------------------------------------------------------------------------------------------------

} // namespace Concurrency
} // namespace Bt

