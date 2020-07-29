//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::MqttSubscription
//
//*************************************************************************************************

#include "Bt/Protocols/MqttSubscription.h"

namespace Bt {
namespace Protocols {

MqttSubscription::MqttSubscription(Concurrency::I_ExecutionContext& pExecutionContext, I_MqttController& pMqttController, std::function<void(std::shared_ptr<MqttMessage>)> pCallback ,const std::string& pTopic, int pQos)
: mExecutionContext(pExecutionContext), mMqttController(pMqttController), mCallback(pCallback), mTopic(pTopic), mQos(pQos){
   mMqttController.add(*this);
}

MqttSubscription::~MqttSubscription() {

}

void MqttSubscription::onMessage(std::shared_ptr<MqttMessage> pMessage) {
   if(mCallback) {
      mExecutionContext.call([this,pMessage](){mCallback(pMessage);});
      ;
   }
}

} // namespace Protocols
} // namespace Bt
