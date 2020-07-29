//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Events::EventLoopSubscription
//
//*************************************************************************************************

#include "Bt/Events/EventLoopSubscription.h"

#include "Bt/Events/Tag.h"

namespace Bt {
namespace Events {



EventLoopSubscription::EventLoopSubscription(I_EventLoop& pEventLoop, esp_event_base_t pEventBase, int32_t pEventId, std::function<void(esp_event_base_t,int32_t,void*)> pCallback)
: mEventLoop(pEventLoop), mEventBase(pEventBase), mEventId(pEventId), mCallback(pCallback), mInstance()  {
   ESP_ERROR_CHECK(mEventLoop.registerHandler(mEventBase, mEventId, &EventLoopSubscription::eventLoopStaticHandler, this, &mInstance));
}

EventLoopSubscription::~EventLoopSubscription() {
   ESP_ERROR_CHECK(mEventLoop.unregisterHandler(mEventBase, mEventId, mInstance));
}

void EventLoopSubscription::eventLoopHandler(esp_event_base_t pEventBase,
                                             int32_t pEventId,
                                             void* pEventData) {
   if(mCallback) {
     mCallback(pEventBase,pEventId,pEventData);
   } else {
      ESP_LOGE(TAG, "Failed to dispatch event loop event: mCallback not valid");
   }
}

void EventLoopSubscription::eventLoopStaticHandler(void* pEventHandlerArg,
                                                   esp_event_base_t pEventBase,
                                                   int32_t pEventId,
                                                   void* pEventData) {
   ESP_LOGD(TAG, "Dispatch event loop event: %s %d", pEventBase, pEventId);
   if(pEventHandlerArg != nullptr) {
      static_cast<EventLoopSubscription*>(pEventHandlerArg)->eventLoopHandler(pEventBase, pEventId, pEventData);
   } else {
      ESP_LOGE(TAG, "Failed to dispatch event loop event: pEventHandlerArg == nullptr");
   }
}

} // namespace Events
} // namespace Bt
