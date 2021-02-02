//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::MqttConfigurationConnector
//
//*************************************************************************************************

#include "Bt/AlarmClock/MqttConfigurationConnector.h"

#include <ArduinoJson.h>

#include <Bt/Protocols/Mqtt/Mqtt.h>

#include "Bt/AlarmClock/Tag.h"


namespace Bt {
namespace AlarmClock {

MqttConfigurationConnector::MqttConfigurationConnector(Concurrency::I_ExecutionContext& pExecutionContext, ConfigurationController& pConfigurationController, Bt::Protocols::I_MqttController& pMqttController)
: mExecutionContext(pExecutionContext)
, mConfigurationController(pConfigurationController)
, mMqttController(pMqttController)
, mMqttGetSubscription(pExecutionContext, pMqttController, "btAlarmClock/devOne/request/config/+/get",1, [this](auto pMessage){onMqttGetMessage(pMessage);})
, mMqttSetSubscription(pExecutionContext, pMqttController, "btAlarmClock/devOne/request/config/+/set",1, [this](auto pMessage){onMqttSetMessage(pMessage);}){

}

MqttConfigurationConnector::~MqttConfigurationConnector() {

}

void MqttConfigurationConnector::onMqttGetMessage(std::shared_ptr<Bt::Protocols::MqttMessage> pMessage) {
   ESP_LOGI(TAG, "MqttConfigurationConnector get request %s => %s", pMessage->topic.c_str(), pMessage->data.c_str());
   auto parts = Protocols::Mqtt::split(pMessage->topic);
   auto key = parts[parts.size()-2];
   auto configuration = mConfigurationController.loadConfiguration(key);

   std::string response;
   if(configuration != nullptr) {
      serializeJson(*configuration, response);
   } else {
      response = "{\"status\":\"configuration not found\"}";
   }

   parts[2] = "response";
   std::string topic = Protocols::Mqtt::join(parts);
   ESP_LOGI(TAG, "MqttConfigurationConnector get response %s => %s", topic.c_str(), response.c_str());
   mMqttController.publish(topic.c_str(), response);

}

void MqttConfigurationConnector::onMqttSetMessage(std::shared_ptr<Bt::Protocols::MqttMessage> pMessage) {
   ESP_LOGI(TAG, "MqttConfigurationConnector set message %s => %s", pMessage->topic.c_str(), pMessage->data.c_str());
   auto parts = Protocols::Mqtt::split(pMessage->topic);
   auto key = parts[parts.size()-2];
   DynamicJsonDocument document(5*1024);
   DeserializationError error = deserializeJson(document, pMessage->data);

   std::string response;
   if (error) {
      ESP_LOGW(TAG, "deserializeJson() failed: %s ", error.c_str());
      response = std::string("{\"status\":\"") + "deserializeJson() failed: " + error.c_str() + "\"}";
   } else {
      JsonVariant variant = document.as<JsonVariant>();
      auto result = mConfigurationController.handle(key, variant);
      response = std::string("{\"status\":\"") + std::get<1>(result) + "\"}";
   }


   parts[2] = "response";
   std::string topic = Protocols::Mqtt::join(parts);
   ESP_LOGI(TAG, "MqttConfigurationConnector get response %s => %s", topic.c_str(), response.c_str());
   mMqttController.publish(topic.c_str(), response);
}

} // namespace AlarmClock
} // namespace Bt
