//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::MqttController
//
//*************************************************************************************************

#ifndef INC__Bt_Protocols_MqttController__h
#define INC__Bt_Protocols_MqttController__h

#include <functional>
#include <list>
#include <map>
#include <string>

#include <mqtt_client.h>

#include <Bt/Events/EventLoopSubscription.h>

#include "Bt/Protocols/I_MqttController.h"

namespace Bt {
namespace Protocols {

class MqttController : public I_MqttController
{
   public:
      MqttController(Events::I_EventLoop& pEventLoop, std::function<void(esp_mqtt_client_config_t&)> pConfigure = nullptr);
      MqttController(const MqttController&) = delete;
      MqttController& operator=(const MqttController&) = delete;
      ~MqttController();

      virtual void publish(const char* pTopic, const std::string& pMsg, int pQos = 0, bool pRetain = false);

      virtual void add(MqttSubscription& pSubscription);
      virtual void remove(MqttSubscription& pSubscription);


   private:
      enum State {INITIAL};

      enum class SubscriptionState{UNSUBSCRIBED, SUBSCRIBING, SUBSCRIBED, UNSUBSCRIBING };
      struct SubscriptionInfo{
            SubscriptionState state = SubscriptionState::UNSUBSCRIBED;
            int id = -1;
      };

      static void mqttEventHandler(void* pHandlerArg, esp_event_base_t pEventBase, int32_t pEventId, void* pEventData);

      void onIpEvent(ip_event_t pEvent, void* pEventData);
      void onMqttEvent(esp_mqtt_event_handle_t pEvent);

      void onConnected();
      void onDisconnected();
      void onSubscribed(int msgId);
      void onData(esp_mqtt_event_handle_t pEvent);


      void subscribe(MqttSubscription* pSubscription);

      std::string mBrokerUri;
      Events::EventLoopSubscription mIpEvents;
      esp_mqtt_client_handle_t mClientHandle;

      std::list<MqttSubscription*> mUnsubscribed;
      std::map<int,MqttSubscription*> mSubscribing;
      std::list<MqttSubscription*> mSubscribed;
      std::map<int,MqttSubscription*> mUnsubscribing;

      std::string mLastOnDataTopic;


};

} // namespace Protocols
} // namespace Bt

#endif // INC__Bt_Protocols_MqttController__h
