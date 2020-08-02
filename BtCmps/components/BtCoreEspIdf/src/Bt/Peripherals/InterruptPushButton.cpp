//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::InterruptPushButton
//
//*************************************************************************************************

#include "Bt/Peripherals/InterruptPushButton.h"

#include <functional>

#include <freertos/task.h>

#include <Bt/Peripherals/Tag.h>

namespace Bt {
namespace Peripherals {

namespace {

   std::once_flag sCreateDispatcher;
   QueueHandle_t sQueue;
   TaskHandle_t sTask;


   struct DispatchEvent {
      InterruptPushButton* mButton;
      InterruptPushButton::Event mEvent;   
   };

   void runDispatcher(void* pContext) {
      while (true) {
         DispatchEvent event;
         if(xQueueReceive(sQueue, &event, portMAX_DELAY)) {
            event.mButton->onDispatch(event.mEvent);
         }  
      }
   }

   void createDispatcher() {
      sQueue = xQueueCreate(20, sizeof(DispatchEvent));
      if(sQueue == nullptr) {
         ESP_LOGE(TAG, "failed to create queue");
         return;
      }
      if(xTaskCreate(runDispatcher, "InterruptDispatcher", 2*1024, nullptr, 10, &sTask) != pdPASS) {
         ESP_LOGE(TAG, "failed to create task");
      }
   }

}

void IRAM_ATTR InterruptPushButton::onInterruptStatic(void* pArg) {
   if(pArg != nullptr) {
      static_cast<InterruptPushButton*>(pArg)->onInterrupt();
   }
}

InterruptPushButton::InterruptPushButton(Concurrency::I_SchedulingExecutionContext& pExecutionContext, gpio_num_t pPin, std::function<void(Event)> pCallback, uint32_t mDebounceTimeout)
: mExecutionContext(pExecutionContext), mPin(pPin), mDebounceTimeout(mDebounceTimeout) {
   mCallback = pCallback;
   gpio_pad_select_gpio(mPin);
   ESP_ERROR_CHECK(gpio_set_direction(mPin, GPIO_MODE_INPUT));
   ESP_ERROR_CHECK(gpio_set_pull_mode(mPin, GPIO_PULLUP_ONLY));
   ESP_ERROR_CHECK(gpio_set_intr_type(mPin, GPIO_INTR_ANYEDGE));

   std::call_once(sCreateDispatcher,[](){
      createDispatcher();
      ESP_ERROR_CHECK(gpio_install_isr_service(0));
   });
   std::function<void(void)> callback = [this](){onInterrupt();};
   ESP_ERROR_CHECK(gpio_isr_handler_add(mPin, &InterruptPushButton::onInterruptStatic, this));

   mState = (gpio_get_level(mPin) == 1) ? State::RELEASED : State::PRESSED;
   ESP_LOGI(TAG, "initial state %d", Core::asInteger(mState.load()));
}

InterruptPushButton::~InterruptPushButton() {
   gpio_isr_handler_remove(mPin);
}

void InterruptPushButton::onDispatch(Event pEvent) {
   if(mCallback) {
      mExecutionContext.call([this,pEvent]{
         mCallback(pEvent);
      });
   }   
}

void InterruptPushButton::onInterrupt() {
   // State in = mState;
   bool pressed = gpio_get_level(mPin) == 0;
   unsigned long now = (esp_timer_get_time() / 1000ULL);
   unsigned long diff = now - mLastChange;
   switch(mState){
      case State::RELEASED : {
         if(pressed) {
            mState = State::TO_PRESSED;
            mLastChange = now;
            postPressed(); 
         }   
      }break;
      case State::TO_PRESSED : {
         if(diff < mDebounceTimeout) {
            mLastChange = now;   
         } else {
            if(pressed) {
               mState = State::PRESSED;
            } else {
               mState = State::TO_RELEASED;
               mLastChange = now;
               postReleased();   
            }
         }
      }break;
      case State::PRESSED : {
         if(!pressed) {
            mState = State::TO_RELEASED;
            mLastChange = now;
            postReleased(); 
         } 
      }break;
      case State::TO_RELEASED : {
         if(diff < mDebounceTimeout) {
            mLastChange = now;   
         } else {
            if(!pressed) {
               mState = State::RELEASED;
            } else {
               mState = State::TO_PRESSED;
               mLastChange = now;
               postPressed();   
            }
         }
      }break;
   }
   //isr_log_i("pin=%d in=%d out=%d diff=%u millis=%u", !pressed, in, mState.load(), diff, now);
}

void InterruptPushButton::postReleased() {
   DispatchEvent event {this, Event::RELEASED};
   auto result = xQueueSendToBackFromISR(sQueue, &event, nullptr);
   if( result != pdPASS) {
      ESP_LOGE(TAG, "xQueueSendToBackFromISR failed");
   }
}

void InterruptPushButton::postPressed() {
   DispatchEvent event {this, Event::PRESSED};
   auto result = xQueueSendToBackFromISR(sQueue, &event, nullptr);
   if( result != pdPASS) {
      ESP_LOGE(TAG, "IxQueueSendToBackFromISR failed");
   }
}

} // namespace Peripherals
} // namespace Bt

