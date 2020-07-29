//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::BlockingQueue
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_BlockingQueue__h
#define INC__Bt_Concurrency_BlockingQueue__h

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Bt {
namespace Concurrency {

template <typename T>
class BlockingQueue
{
   public:

      bool empty() const {
         std::lock_guard<std::mutex> lock(mMutex);
         return mQueue.empty();
      }

      typename std::queue<T>::size_type size() const {
         std::lock_guard<std::mutex> lock(mMutex);
         return mQueue.size();
      }

      void push(T pValue)
      {
         std::lock_guard<std::mutex> lock(mMutex);
         mQueue.push(std::move(pValue));
         mConditionVariable.notify_one();
      }

      void pop(T& pValue)
      {
         std::unique_lock<std::mutex> lock(mMutex);
         mConditionVariable.wait(lock, [this]{return !mQueue.empty();});
         pValue = std::move(mQueue.front());
         mQueue.pop();
      }

      bool tryPop(T& pValue)
      {
         std::lock_guard<std::mutex> lock(mMutex);
         if (mQueue.empty()) {
            return false;
         }
         pValue = std::move(mQueue.front());
         mQueue.pop();
         return true;
      }

      template< class Rep, class Period>
      bool tryPop(T& pValue, const std::chrono::duration<Rep, Period>& pTimeout)
      {
         std::unique_lock<std::mutex> lock(mMutex);
         if(!mConditionVariable.wait_for(lock, pTimeout, [this]{return !mQueue.empty();})){
            return false;
         }
         pValue = std::move(mQueue.front());
         mQueue.pop();
         return true;
      }

      T* peek()
      {
         std::lock_guard<std::mutex> lock(mMutex);
         if (mQueue.empty()) {
            return nullptr;
         }
         return &mQueue.front();
      }

      const T* peek() const
      {
         std::lock_guard<std::mutex> lock(mMutex);
         if (mQueue.empty()) {
            return nullptr;
         }
         return &mQueue.front();
      }

      void wait()
      {
         std::unique_lock<std::mutex> lock(mMutex);
         mConditionVariable.wait(lock, [this]{return !mQueue.empty();});
      }

      template< class Rep, class Period>
      bool waitFor(const std::chrono::duration<Rep, Period>& pTimeout)
      {
         std::unique_lock<std::mutex> lock(mMutex);
         if(!mConditionVariable.wait_for(lock, pTimeout, [this]{return !mQueue.empty();})){
            return false;
         }
         return true;
      }
      
   private:
      mutable std::mutex mMutex;
      std::queue<T> mQueue;
      std::condition_variable mConditionVariable;
};


} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_BlockingQueue__h
