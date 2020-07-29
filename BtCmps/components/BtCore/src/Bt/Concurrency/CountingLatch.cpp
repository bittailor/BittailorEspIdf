//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::CountingLatch
//
//*************************************************************************************************

#include "Bt/Concurrency/CountingLatch.h"

#include <system_error>

namespace Bt {
namespace Concurrency {

CountingLatch::CountingLatch(unsigned int pInitialCount) : mCount(pInitialCount) {

}

CountingLatch::~CountingLatch() {

}

void CountingLatch::countUp() {
   std::unique_lock<std::mutex> lock(mMutex);
   mCount++;
}

void CountingLatch::countDown() {
   std::unique_lock<std::mutex> lock(mMutex);
   if (mCount <= 0) {
      throw std::system_error(std::make_error_code(std::errc::invalid_argument));
   }
   if (--mCount == 0) {
      mConditionVariable.notify_all();
   }
}

void CountingLatch::wait() {
   std::unique_lock<std::mutex> lock(mMutex);
   mConditionVariable.wait(lock,[this]{return mCount == 0;});
}

} // namespace Concurrency
} // namespace Bt
