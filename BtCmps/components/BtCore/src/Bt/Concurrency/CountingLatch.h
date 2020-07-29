//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::CountingLatch
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_CountingLatch__h
#define INC__Bt_Concurrency_CountingLatch__h

#include <mutex>
#include <condition_variable>

namespace Bt {
namespace Concurrency {

class CountingLatch 
{
   public:
      explicit CountingLatch(unsigned int pInitialCount);
   	  CountingLatch(const CountingLatch&) = delete;
      CountingLatch& operator=(const CountingLatch&) = delete;
      ~CountingLatch();

      unsigned int count() {
         std::unique_lock<std::mutex> lock(mMutex);
         return mCount;   
      }

      void countUp();
      void countDown();
      void wait();

      template< class Rep, class Period>
      bool waitFor(const std::chrono::duration<Rep, Period>& pTimeout) {
         std::unique_lock<std::mutex> lock(mMutex);
         return mConditionVariable.wait_for(lock, pTimeout, [this]{return mCount == 0;});
      }

   private:

      unsigned int mCount;
      std::mutex mMutex;
      std::condition_variable mConditionVariable;

};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_CountingLatch__h
