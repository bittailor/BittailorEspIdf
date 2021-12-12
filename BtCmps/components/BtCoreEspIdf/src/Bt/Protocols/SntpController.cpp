//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::SntpController
//
//*************************************************************************************************

#include "Bt/Protocols/SntpController.h"

#include <stdexcept>

#include <Bt/Core/I_Time.h>
#include <Bt/Events/Events.h>

#include "Bt/Protocols/Tag.h"


namespace Bt {
namespace Protocols {
namespace {

   //static  NTP_SERVERS "0.ch.pool.ntp.org", "1.ch.pool.ntp.org", "pool.ntp.org"

   std::function<void()> sTimeSyncNotificationCallback;
   void timeSyncNotificationCallback(timeval *tv)
   {
      if(sTimeSyncNotificationCallback) {
         sTimeSyncNotificationCallback();
      }

   }
}

SntpController::SntpController(Events::I_EventLoop& pEventLoop)
: mIpEvents(pEventLoop, IP_EVENT, IP_EVENT_STA_GOT_IP,[this](esp_event_base_t pEventBase, int32_t pEventId, void* pEventData){onIpEvent(static_cast<ip_event_t>(pEventId),pEventData);})
{
   if(sTimeSyncNotificationCallback) {
      ESP_LOGE(TAG, "Only one active instance of SntpController allowed");
      throw std::logic_error("Only one active instance of SntpController allowed");
   }

   sTimeSyncNotificationCallback = [this](){onTimeSyncNotification();};
   mStatus = sntp_get_sync_status();
   ESP_LOGI(TAG, "Initial SNTP status %d", sntp_get_sync_status());
   sntp_setoperatingmode(SNTP_OPMODE_POLL);
   sntp_setservername(0, "0.ch.pool.ntp.org");
   sntp_set_time_sync_notification_cb(timeSyncNotificationCallback);

}

SntpController::~SntpController() {
   sTimeSyncNotificationCallback = std::function<void()>();
}

void SntpController::onIpEvent(ip_event_t pEvent, void* pEventData) {
   if(pEvent == IP_EVENT_STA_GOT_IP) {
      if (!sntp_enabled())
      {
         ESP_LOGI(TAG, "SNTP start");
         sntp_init();
      } else {
         ESP_LOGI(TAG, "SNTP restart");
         sntp_restart();
      }
   }
}

void SntpController::onTimeSyncNotification() {
   sntp_sync_status_t status = sntp_get_sync_status();
   ESP_LOGI(TAG, "Notification of a time synchronization event [status = %d]", status);
   if(status == SNTP_SYNC_STATUS_COMPLETED) {
      if(mStatus != SNTP_SYNC_STATUS_COMPLETED) {
         mStatus = status;
         ESP_LOGI(TAG, "SNTP initial sync completed");
         Events::publish(Core::I_Time::TimeSyncCompleted{});
      }
   }
}

} // namespace Protocols
} // namespace Bt
