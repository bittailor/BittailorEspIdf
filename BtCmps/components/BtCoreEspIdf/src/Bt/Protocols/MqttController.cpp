//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::MqttController
//
//*************************************************************************************************

#include "Bt/Protocols/MqttController.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <sdkconfig.h>

#include <Bt/Events/Events.h>
#include "Bt/Protocols/Mqtt/Mqtt.h"
#include "Bt/Protocols/MqttSubscription.h"
#include "Bt/Protocols/Tag.h"

namespace Bt {
namespace Protocols {
namespace {
   
   class MqttSubscriptionVisitor : public I_MqttSubscriptionVisitor  {
      public:
         
         virtual void visit(MqttMessageSubscription& pSubscription) {
            mMessageSubscriptions.push_back(&pSubscription);
         } 
         virtual void visit(MqttRawSubscription& pSubscription) {
            mRawSubscriptions.push_back(&pSubscription);
         }

         bool empty() const {
            return mMessageSubscriptions.empty() && mRawSubscriptions.empty(); 
         }

         std::list<MqttMessageSubscription*> mMessageSubscriptions;   
         std::list<MqttRawSubscription*> mRawSubscriptions;   
   }; 

} // namespace 

void MqttController::mqttEventHandler(void* pHandlerArg, esp_event_base_t pEventBase, int32_t pEventId, void* pEventData) {
   if(pHandlerArg == nullptr) {
      ESP_LOGE(TAG, "MqttController pHandlerArg is nullptr");
      return;
   }
   static_cast<MqttController*>(pHandlerArg)->onMqttEvent(static_cast<esp_mqtt_event_handle_t>(pEventData));
}


MqttController::MqttController(Events::I_EventLoop& pEventLoop, std::function<void(esp_mqtt_client_config_t&)> pConfigure)
: mBrokerUri(CONFIG_BITTAILOR_MQTT_URI)
, mIpEvents(pEventLoop, IP_EVENT, IP_EVENT_STA_GOT_IP,[this](esp_event_base_t pEventBase, int32_t pEventId, void* pEventData){onIpEvent(static_cast<ip_event_t>(pEventId),pEventData);}) {
   esp_mqtt_client_config_t cfg = {};
   cfg.uri = mBrokerUri.c_str();
   cfg.username = CONFIG_BITTAILOR_MQTT_USERNAME;
   cfg.password = CONFIG_BITTAILOR_MQTT_PASSWORD;
   if(pConfigure) {
      pConfigure(cfg);
   }
   mClientHandle = esp_mqtt_client_init(&cfg);
   if(mClientHandle == nullptr) {
      throw std::runtime_error("Failed to init MQTT client");
   }
   ESP_ERROR_CHECK(esp_mqtt_client_register_event(mClientHandle, MQTT_EVENT_ANY, &MqttController::mqttEventHandler, this));
}

MqttController::~MqttController() {
   ESP_ERROR_CHECK(esp_mqtt_client_destroy(mClientHandle));
}

void MqttController::publish(const char* pTopic, const std::string& pMsg, int pQos, bool pRetain) {
   esp_mqtt_client_publish(mClientHandle, pTopic, pMsg.c_str(), pMsg.length(), pQos, pRetain);
}

void MqttController::add(MqttSubscription& pSubscription) {
   mUnsubscribed.push_back(&pSubscription);
}

void MqttController::remove(MqttSubscription& pSubscription) {

}

void MqttController::onConnected() {
   std::list<MqttSubscription*> toSubscribe;
   toSubscribe.swap(mUnsubscribed);
   for (auto& subscription : toSubscribe) {
         subscribe(subscription);
   }
   Bt::Events::publish(I_MqttController::Connected{});
}

 void MqttController::onDisconnected() {
   mUnsubscribed.insert(std::end(mUnsubscribed),std::begin(mSubscribed),std::end(mSubscribed));
   mSubscribed.clear();
   for (auto &&i : mSubscribing)
   {
      mUnsubscribed.push_back(i.second);   
   }
   mSubscribing.clear();
   mUnsubscribing.clear();
   Bt::Events::publish(I_MqttController::Disconnected{}); 
 }

void MqttController::onSubscribed(int msgId) {
   auto iter = mSubscribing.find(msgId);
   if (iter == std::end(mSubscribing)) {
      ESP_LOGE(TAG, "onSubscribed msgId %d not found", msgId);
      return;
   }
   MqttSubscription* subscription = iter->second;
   mSubscribing.erase(iter);
   mSubscribed.push_back(subscription);
}

void MqttController::onData(esp_mqtt_event_handle_t pEvent) {
   if(pEvent->topic_len > 0) {
      mLastOnDataTopic = std::string{pEvent->topic, static_cast<size_t>(pEvent->topic_len)};
   }

   MqttSubscriptionVisitor mVisitor;
   for (MqttSubscription* subscription  : mSubscribed)
   {
      if(Mqtt::topicMatchesSubscription(subscription->topic(), mLastOnDataTopic)){
         subscription->accept(mVisitor);
      }   
   }

   if(mVisitor.empty()) {
      ESP_LOGW(TAG, "no matching subscription found for TOPIC=%s", mLastOnDataTopic.c_str());
      return;
   }

   if(!mVisitor.mRawSubscriptions.empty()) {
      RawMqttMessage rawMessage{
         pEvent->topic, static_cast<size_t>(pEvent->topic_len),
         pEvent->data, static_cast<size_t>(pEvent->data_len),
         static_cast<size_t>(pEvent->current_data_offset),
         static_cast<size_t>(pEvent->total_data_len)};
      for(MqttRawSubscription* subscription : mVisitor.mRawSubscriptions) {
         subscription->onRawMessage(rawMessage);
      } 
   }

   if(!mVisitor.mMessageSubscriptions.empty()) {
      std::shared_ptr<MqttMessage> message = std::make_shared<MqttMessage>(
         mLastOnDataTopic.c_str(), mLastOnDataTopic.length(), 
         pEvent->data, static_cast<size_t>(pEvent->data_len),
         pEvent->current_data_offset,
         pEvent->total_data_len);
      for(MqttMessageSubscription* subscription : mVisitor.mMessageSubscriptions) {
         subscription->onMessage(message);
      }
   }   
}

void MqttController::subscribe(MqttSubscription* pSubscription) {
   int msgId = esp_mqtt_client_subscribe(mClientHandle, pSubscription->topic().c_str(), pSubscription->qos());
   if(msgId == -1) {
      ESP_LOGE(TAG, "failed to subscribe %s %d",pSubscription->topic().c_str(), pSubscription->qos());
      mUnsubscribed.push_back(pSubscription);
      return;
   }
   mSubscribing[msgId] = pSubscription;
}

void MqttController::onIpEvent(ip_event_t pEvent, void* pEventData) {
   if(pEvent == IP_EVENT_STA_GOT_IP) {
      ESP_LOGI(TAG, "MQTT start");
      ESP_ERROR_CHECK(esp_mqtt_client_start(mClientHandle));
   }
}

void MqttController::onMqttEvent(esp_mqtt_event_handle_t pEvent) {
   switch (pEvent->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            onConnected();
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            onDisconnected();
            break;
        case MQTT_EVENT_SUBSCRIBED:
            onSubscribed(pEvent->msg_id);
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", pEvent->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", pEvent->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", pEvent->msg_id);
            break;
        case MQTT_EVENT_DATA:
            /*
            - msg_id               message id
            - topic                pointer to the received topic
            - topic_len            length of the topic
            - data                 pointer to the received data
            - data_len             length of the data for this event
            - current_data_offset  offset of the current data for this event
            - total_data_len       total length of the data received
            */
            ESP_LOGD(TAG, "MQTT_EVENT_DATA");
            ESP_LOGD(TAG, " - Id=%d topic_len=%d , data_len=%d, current_data_offset=%d, total_data_len=%d", pEvent->msg_id, pEvent->topic_len, pEvent->data_len, pEvent->current_data_offset, pEvent->total_data_len);
            ESP_LOGD(TAG, " - TOPIC=%.*s", pEvent->topic_len, pEvent->topic);
            //ESP_LOGI(TAG, " - DATA=%.*s", pEvent->data_len, pEvent->data);
            onData(pEvent);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", pEvent->event_id);
            break;
    }
}


} // namespace Protocols
} // namespace Bt
