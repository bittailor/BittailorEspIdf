//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::CountdownLatch
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_CountdownLatch__h
#define INC__Bt_Concurrency_CountdownLatch__h

#include <mutex>
#include <condition_variable>

namespace Bt {
namespace Concurrency {

class CountdownLatch 
{
   public:
      explicit CountdownLatch(unsigned int pCount);
   	  CountdownLatch(const CountdownLatch&) = delete;
      CountdownLatch& operator=(const CountdownLatch&) = delete;
      ~CountdownLatch();

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

#endif // INC__Bt_Concurrency_CountdownLatch__h
