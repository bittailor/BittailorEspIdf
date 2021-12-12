//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::SntpController
//
//*************************************************************************************************

#ifndef INC__Bt_Protocols_SntpController__h
#define INC__Bt_Protocols_SntpController__h

#include <esp_sntp.h>

#include <Bt/Events/I_EventLoop.h>
#include <Bt/Events/EventLoopSubscription.h>

#include "Bt/Protocols/I_SntpController.h"

namespace Bt {
namespace Protocols {

class SntpController : public I_SntpController
{
   public:
      SntpController(Events::I_EventLoop& pEventLoop);
      SntpController(const SntpController&) = delete;
      SntpController& operator=(const SntpController&) = delete;
      ~SntpController();

   private:
      void onIpEvent(ip_event_t pEvent, void* pEventData);
      void onTimeSyncNotification();


      Events::EventLoopSubscription mIpEvents;
      sntp_sync_status_t mStatus;
      


};

} // namespace Protocols
} // namespace Bt

#endif // INC__Bt_Protocols_SntpController__h
