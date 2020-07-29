//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::CountdownLatch
//
//*************************************************************************************************

#include "Bt/Concurrency/CountdownLatch.h"

#include <system_error>

namespace Bt {
namespace Concurrency {

CountdownLatch::CountdownLatch(unsigned int pCount) : mCount(pCount) {
}

CountdownLatch::~CountdownLatch() {

}

void CountdownLatch::countDown() {
   std::unique_lock<std::mutex> lock(mMutex);
   if (mCount <= 0) {
      throw std::system_error(std::make_error_code(std::errc::invalid_argument));
   }
   if (--mCount == 0) {
      mConditionVariable.notify_all();
   }
}

void CountdownLatch::wait() {
   std::unique_lock<std::mutex> lock(mMutex);
   mConditionVariable.wait(lock,[this]{return mCount == 0;});
}

} // namespace Concurrency
} // namespace Bt
