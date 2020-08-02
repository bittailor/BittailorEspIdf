//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::InterruptPushButton
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_InterruptPushButton__h
#define INC__Bt_Peripherals_InterruptPushButton__h

#include <stdint.h>
#include <atomic>
#include <functional>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <driver/gpio.h>

#include <Bt/Concurrency/SchedulingExecutionContext.h>

namespace Bt {
namespace Peripherals {

class InterruptPushButton
{
   public:
      enum class Event {RELEASED, PRESSED};
      
      InterruptPushButton(Concurrency::I_SchedulingExecutionContext& pExecutionContext, gpio_num_t pPin, std::function<void(Event)> pCallback, uint32_t mDebounceTimeout = 20);
      InterruptPushButton(const InterruptPushButton&) = delete;
      InterruptPushButton& operator=(const InterruptPushButton&) = delete;
      ~InterruptPushButton();

      void begin(std::function<void(Event)> pCallback);
      void end();

      void onDispatch(Event pEvent);

   private:
      static void IRAM_ATTR onInterruptStatic(void* pArg);

      void onInterrupt();
      void postReleased();
      void postPressed();
      
      enum class State {RELEASED, TO_PRESSED, PRESSED, TO_RELEASED};

      Concurrency::I_SchedulingExecutionContext& mExecutionContext;
      gpio_num_t mPin;
      uint32_t mDebounceTimeout;
      std::atomic<State> mState;
      unsigned long mLastChange;
      std::function<void(Event)> mCallback;

   
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_InterruptPushButton__h

