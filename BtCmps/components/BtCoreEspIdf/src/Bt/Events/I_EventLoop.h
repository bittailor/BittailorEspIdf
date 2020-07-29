//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Events::I_EventLoop
//
//*************************************************************************************************

#ifndef INC__Bt_Events_I_EventLoop__h
#define INC__Bt_Events_I_EventLoop__h

#include <esp_err.h>
#include <esp_event.h>

namespace Bt {
namespace Events {

class I_EventLoop {
   public:
      virtual ~I_EventLoop() {}

      virtual esp_err_t registerHandler(esp_event_base_t pEventBase, int32_t pEventId, esp_event_handler_t pEventHandler, void* pEventHandlerArg, esp_event_handler_instance_t* pInstance) = 0;
      virtual esp_err_t unregisterHandler(esp_event_base_t pEventBase, int32_t pEventId, esp_event_handler_instance_t pInstance) = 0;

};

} // namespace Events
} // namespace Bt

#endif // INC__Bt_Events_I_EventLoop__h
