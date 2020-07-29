//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Events::DefaultEventLoop
//
//*************************************************************************************************

#include "Bt/Events/DefaultEventLoop.h"

#include <mutex>

#include <esp_event.h>

namespace Bt {
namespace Events {
namespace {
   std::once_flag sOnceFlag;
}

DefaultEventLoop::DefaultEventLoop() {
   std::call_once(sOnceFlag,[]() {
      ESP_ERROR_CHECK(esp_event_loop_create_default());
   });
}

DefaultEventLoop::~DefaultEventLoop() {

}

esp_err_t DefaultEventLoop::registerHandler(esp_event_base_t pEventBase, int32_t pEventId, esp_event_handler_t pEventHandler, void* pEventHandlerArg, esp_event_handler_instance_t *pInstance) {
   return esp_event_handler_instance_register(pEventBase, pEventId, pEventHandler, pEventHandlerArg, pInstance);
}


esp_err_t DefaultEventLoop::unregisterHandler(esp_event_base_t pEventBase, int32_t pEventId, esp_event_handler_instance_t pInstance) {
   return esp_event_handler_instance_unregister(pEventBase, pEventId, pInstance);
}



} // namespace Events
} // namespace Bt
