//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::I_ExecutionContext
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_I_ExecutionContext__h
#define INC__Bt_Concurrency_I_ExecutionContext__h

#include <chrono>
#include <functional>
#include <future>
#include <type_traits>
#include <memory>

namespace Bt {
namespace Concurrency {

class I_ExecutionContext {
   public:
      static I_ExecutionContext* current();

      virtual ~I_ExecutionContext() {}
      
      virtual void call(std::function<void()> pFunction) = 0;
      virtual void ensureCallOnContext(std::function<void()> pFunction);

      template<typename Fn, typename... Args>
      std::future<typename std::result_of<Fn(Args...)>::type> callTask(Fn&& pFunction, Args&&... pArgs) {
         typedef typename std::result_of<Fn(Args...)>::type R;
         std::shared_ptr<std::promise<R>> promise = std::make_shared<std::promise<R>>();
         std::future<R> future = promise->get_future();
         std::function<R()> function(std::bind(std::forward<Fn>(pFunction),std::forward<Args>(pArgs)...));
         std::function<void()> voidFunction = std::bind(&I_ExecutionContext::callTaskInternal<R>, this, function, promise);
         call(voidFunction);
         return future;
      }

   protected:
      I_ExecutionContext* setCurrentExecutionContext(I_ExecutionContext* pCurrent);                 

   private:

      template<typename R>
      void callTaskInternal(std::function<R()> iFunction, std::shared_ptr<std::promise<R>> pPromise) {
          try {
              pPromise->set_value(iFunction());
          } catch(...) {
              pPromise->set_exception(std::current_exception());
          }
      }
};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_I_ExecutionContext__h
