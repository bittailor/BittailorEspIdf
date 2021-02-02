//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::MqttSubscription
//
//*************************************************************************************************

#include "Bt/Protocols/MqttSubscription.h"

namespace Bt {
namespace Protocols {

MqttSubscription::MqttSubscription(I_MqttController& pMqttController,const std::string& pTopic, int pQos)
: mMqttController(pMqttController), mTopic(pTopic), mQos(pQos){
   mMqttController.add(*this);
}

MqttSubscription::~MqttSubscription() {
   mMqttController.remove(*this);
}



MqttMessageSubscription::MqttMessageSubscription(Concurrency::I_ExecutionContext& pExecutionContext, I_MqttController& pMqttController ,const std::string& pTopic, int pQos, std::function<void(std::shared_ptr<MqttMessage>)> pCallback)
:  MqttSubscription(pMqttController, pTopic, pQos), mExecutionContext(pExecutionContext), mCallback(pCallback) {
}


void MqttMessageSubscription::onMessage(std::shared_ptr<MqttMessage> pMessage) {
   if(mCallback) {
      mExecutionContext.call([this,pMessage](){mCallback(pMessage);});
   }
}

void MqttMessageSubscription::accept(I_MqttSubscriptionVisitor& pVisitor) {
   pVisitor.visit(*this);
}  

MqttRawSubscription::MqttRawSubscription(I_MqttController& pMqttController, const std::string& pTopic, int pQos, std::function<void(const RawMqttMessage& pMessage)> pCallback)
:  MqttSubscription(pMqttController, pTopic, pQos), mCallback(pCallback) {
}


void MqttRawSubscription::onRawMessage(const RawMqttMessage& pMessage) {
   if(mCallback) {
      mCallback(pMessage);   
   }
}

void MqttRawSubscription::accept(I_MqttSubscriptionVisitor& pVisitor) {
   pVisitor.visit(*this);
}  



} // namespace Protocols
} // namespace Bt
