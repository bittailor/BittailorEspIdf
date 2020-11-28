//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::MqttController
//
//*************************************************************************************************

#include "Bt/Protocols/MqttController.h"

#include <stdexcept>
#include <vector>

#include <Bt/Events/Events.h>

#include "Bt/Protocols/Mqtt/Mqtt.h"
#include "Bt/Protocols/MqttSubscription.h"
#include "Bt/Protocols/Tag.h"

namespace Bt {
namespace Protocols {

void MqttController::mqttEventHandler(void* pHandlerArg, esp_event_base_t pEventBase, int32_t pEventId, void* pEventData) {
   if(pHandlerArg == nullptr) {
      ESP_LOGE(TAG, "MqttController pHandlerArg is nullptr");
      return;
   }
   static_cast<MqttController*>(pHandlerArg)->onMqttEvent(static_cast<esp_mqtt_event_handle_t>(pEventData));
}


MqttController::MqttController(Events::I_EventLoop& pEventLoop, const std::string& pBrokerUri, std::function<void(esp_mqtt_client_config_t&)> pConfigure)
: mBrokerUri(pBrokerUri)
, mIpEvents(pEventLoop, IP_EVENT, IP_EVENT_STA_GOT_IP,[this](esp_event_base_t pEventBase, int32_t pEventId, void* pEventData){onIpEvent(static_cast<ip_event_t>(pEventId),pEventData);}) {
   esp_mqtt_client_config_t cfg = {};
   cfg.uri = mBrokerUri.c_str();
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
   std::string topic{pEvent->topic, static_cast<size_t>(pEvent->topic_len)};

   std::list<MqttSubscription*> macthingSubscriptions;
   std::copy_if(std::begin(mSubscribed),
                std::end(mSubscribed),
                std::back_inserter(macthingSubscriptions),
                [&topic](const MqttSubscription* subscription) {
                     return Mqtt::topicMatchesSubscription(subscription->topic().c_str(), topic.c_str());
                }
   );

   if(macthingSubscriptions.size() < 1) {
      ESP_LOGW(TAG, "no matching subscription found for TOPIC=%s", topic.c_str());
      return;
   }

   std::shared_ptr<MqttMessage> message = std::make_shared<MqttMessage>(pEvent->topic, static_cast<size_t>(pEvent->topic_len), pEvent->data, static_cast<size_t>(pEvent->data_len));
   for(MqttSubscription* subscription :macthingSubscriptions) {
      subscription->onMessage(message);
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
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            ESP_LOGI(TAG, " - Id=%d", pEvent->msg_id);
            ESP_LOGI(TAG, " - TOPIC=%.*s", pEvent->topic_len, pEvent->topic);
            ESP_LOGI(TAG, " - DATA=%.*s", pEvent->data_len, pEvent->data);
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
