//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Events::EventLoopSubscription
//
//*************************************************************************************************

#ifndef INC__Bt_Events_EventLoopSubscription__h
#define INC__Bt_Events_EventLoopSubscription__h

#include <functional>

#include <esp_event.h>

#include "Bt/Events/I_EventLoop.h"


namespace Bt {
namespace Events {

class EventLoopSubscription
{
   public:
      EventLoopSubscription(I_EventLoop& pEventLoop, esp_event_base_t pEventBase, int32_t pEventId, std::function<void(esp_event_base_t,int32_t,void*)> pCallback);
      EventLoopSubscription(const EventLoopSubscription&) = delete;
      EventLoopSubscription& operator=(const EventLoopSubscription&) = delete;
      ~EventLoopSubscription();

   private:
      I_EventLoop& mEventLoop;
      esp_event_base_t mEventBase;
      int32_t mEventId;
      std::function<void(esp_event_base_t,int32_t,void*)> mCallback;
      esp_event_handler_instance_t mInstance;
      
      void eventLoopHandler(esp_event_base_t pEventBase,
                            int32_t pEventId,
                            void* pEventData);

      static void eventLoopStaticHandler(void* pEventHandlerArg,
                                         esp_event_base_t pEventBase,
                                         int32_t pEventId,
                                         void* pEventData);

};

} // namespace Events
} // namespace Bt

#endif // INC__Bt_Events_EventLoopSubscription__h
