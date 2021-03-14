//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Network::WiFiController
//
//*************************************************************************************************

#include "Bt/Network/WiFiController.h"

#include <mutex>
#include <cstring>

#include <sdkconfig.h>

#include "Bt/Network/Tag.h"

namespace Bt {
namespace Network {
namespace {
   std::once_flag sOnceFlag;
}

WiFiController::WiFiController(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Events::I_EventLoop& pEventLoop, const char* pSsid, const char* pPassword)
: mExecutionContext(pExecutionContext)
, mEventLoop(pEventLoop)
, mIpEvents(pEventLoop, IP_EVENT, IP_EVENT_STA_GOT_IP,[this](esp_event_base_t pEventBase, int32_t pEventId, void* pEventData){onIpEvent(static_cast<ip_event_t>(pEventId),pEventData);})
, mWifiEvents(pEventLoop, WIFI_EVENT, ESP_EVENT_ANY_ID,[this](esp_event_base_t pEventBase, int32_t pEventId, void* pEventData){onWifiEvent(static_cast<wifi_event_t>(pEventId),pEventData);})
, mWifiConfig{}
{
   std::call_once(sOnceFlag,[]() {
      ESP_ERROR_CHECK(esp_netif_init());
   });

   esp_netif_create_default_wifi_sta();
   wifi_init_config_t initConfig = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK(esp_wifi_init(&initConfig));

   strcpy((char*)mWifiConfig.sta.ssid, pSsid);
   strcpy((char*)mWifiConfig.sta.password, pPassword);


   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
   ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &mWifiConfig));

   mExecutionContext.call([this](){
      ESP_LOGI(TAG, "Start the WIFI SSID:[%s] password:[%s]", (char*)mWifiConfig.sta.ssid, "******");
      ESP_ERROR_CHECK(esp_wifi_start());

   });

}

WiFiController::~WiFiController() {

}

void WiFiController::onIpEvent(ip_event_t pEvent, void* pEventData) {
   ESP_LOGD(TAG, "onIpEvent: %d", pEvent);
   if(pEvent == IP_EVENT_STA_GOT_IP) {
      ip_event_got_ip_t* gotIpEvent = static_cast<ip_event_got_ip_t*>(pEventData);
      ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP: %d.%d.%d.%d", IP2STR(&gotIpEvent->ip_info.ip));
   }
}

void WiFiController::onWifiEvent(wifi_event_t pEvent, void* pEventData) {
   ESP_LOGI(TAG, "onWifiEvent: %d", pEvent);

   if (pEvent == WIFI_EVENT_STA_START) {
      ESP_LOGI(TAG, "WIFI_EVENT_STA_START => connect()");
      esp_wifi_connect();
   }
   if (pEvent == WIFI_EVENT_STA_CONNECTED) {
        wifi_event_sta_connected_t* staConnectedEvent = static_cast<wifi_event_sta_connected_t*>(pEventData);
        ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED: %s", (char*)staConnectedEvent->ssid);
   }
   if (pEvent == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED => try reconnect");
        esp_wifi_connect();
   }

}

} // namespace Network
} // namespace Bt
