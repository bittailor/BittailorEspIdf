//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Events::DefaultEventLoop
//
//*************************************************************************************************

#ifndef INC__Bt_Events_DefaultEventLoop__h
#define INC__Bt_Events_DefaultEventLoop__h

#include "Bt/Events/I_EventLoop.h"

namespace Bt {
namespace Events {

class DefaultEventLoop : public I_EventLoop
{
   public:
      DefaultEventLoop();
      DefaultEventLoop(const DefaultEventLoop&) = delete;
      DefaultEventLoop& operator=(const DefaultEventLoop&) = delete;
      ~DefaultEventLoop();

      virtual esp_err_t registerHandler(esp_event_base_t pEventBase, int32_t pEventId, esp_event_handler_t pEventHandler, void* pEventHandlerArg, esp_event_handler_instance_t *pInstance);
      virtual esp_err_t unregisterHandler(esp_event_base_t pEventBase, int32_t pEventId, esp_event_handler_instance_t pInstance);

   private:
      
};

} // namespace Events
} // namespace Bt

#endif // INC__Bt_Events_DefaultEventLoop__h
