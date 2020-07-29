//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Network::WiFiController
//
//*************************************************************************************************

#ifndef INC__Bt_Network_WiFiController__h
#define INC__Bt_Network_WiFiController__h

#include <esp_wifi.h>

#include <Bt/Concurrency/I_SchedulingExecutionContext.h>
#include <Bt/Events/I_EventLoop.h>
#include <Bt/Events/EventLoopSubscription.h>

namespace Bt {
namespace Network {

class WiFiController
{
   public:
      WiFiController(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Events::I_EventLoop& pEventLoop, const char* pSsid, const char* pPassword);
      WiFiController(const WiFiController&) = delete;
      WiFiController& operator=(const WiFiController&) = delete;
      ~WiFiController();

   private:
      Concurrency::I_SchedulingExecutionContext& mExecutionContext;
      Events::I_EventLoop& mEventLoop;
      Events::EventLoopSubscription mIpEvents;
      Events::EventLoopSubscription mWifiEvents;
      wifi_config_t mWifiConfig;
      
      void onIpEvent(ip_event_t pEvent, void* pEventData);
      void onWifiEvent(wifi_event_t pEvent, void* pEventData);


};

} // namespace Network
} // namespace Bt

#endif // INC__Bt_Network_WiFiController__h
