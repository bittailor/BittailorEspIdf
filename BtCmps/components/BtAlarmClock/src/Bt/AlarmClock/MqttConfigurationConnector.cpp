//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::MqttConfigurationConnector
//
//*************************************************************************************************

#include "Bt/AlarmClock/MqttConfigurationConnector.h"

#include "Bt/AlarmClock/Tag.h"

namespace Bt {
namespace AlarmClock {

MqttConfigurationConnector::MqttConfigurationConnector(Concurrency::I_ExecutionContext& pExecutionContext, ConfigurationController& pConfigurationController, Bt::Protocols::I_MqttController& pMqttController)
: mExecutionContext(pExecutionContext)
, mConfigurationController(pConfigurationController)
, mMqttGetSubscription(pExecutionContext, pMqttController, [this](auto pMessage){onMqttGetMessage(pMessage);}, "/btAlarmClock/devOne/config/+/get",1)
, mMqttSetSubscription(pExecutionContext, pMqttController, [this](auto pMessage){onMqttSetMessage(pMessage);}, "/btAlarmClock/devOne/config/+/set",1){

}

MqttConfigurationConnector::~MqttConfigurationConnector() {

}

void MqttConfigurationConnector::onMqttGetMessage(std::shared_ptr<Bt::Protocols::MqttMessage> pMessage) {
   ESP_LOGI(TAG, "MqttConfigurationConnector get message %s => %s", pMessage->topic.c_str(), pMessage->data.c_str());




}

void MqttConfigurationConnector::onMqttSetMessage(std::shared_ptr<Bt::Protocols::MqttMessage> pMessage) {
   ESP_LOGI(TAG, "MqttConfigurationConnector set message %s => %s", pMessage->topic.c_str(), pMessage->data.c_str());
}

} // namespace AlarmClock
} // namespace Bt
