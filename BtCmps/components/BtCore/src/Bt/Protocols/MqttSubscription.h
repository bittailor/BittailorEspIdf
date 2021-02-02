//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::MqttSubscription
//
//*************************************************************************************************

#ifndef INC__Bt_Protocols_MqttSubscription__h
#define INC__Bt_Protocols_MqttSubscription__h

#include <functional>
#include <memory>
#include <string>

#include <Bt/Concurrency/I_ExecutionContext.h>

#include "Bt/Protocols/I_MqttController.h"
#include "Bt/Protocols/MqttMessage.h"

namespace Bt {
namespace Protocols {

class MqttMessageSubscription;
class MqttRawSubscription;

class I_MqttSubscriptionVisitor {
   public:
      virtual ~I_MqttSubscriptionVisitor(){};

      virtual void visit(MqttMessageSubscription& pSubscription) = 0; 
      virtual void visit(MqttRawSubscription& pSubscription) = 0; 
};

class MqttSubscription
{
   public:
      MqttSubscription(const MqttSubscription&) = delete;
      MqttSubscription& operator=(const MqttSubscription&) = delete;
      ~MqttSubscription();

      const std::string& topic() const {return mTopic;}
      int qos() const {return mQos;}

      virtual void accept(I_MqttSubscriptionVisitor& pVisitor) = 0;  

   protected:
      MqttSubscription(I_MqttController& pMqttController, const std::string& pTopic, int pQos);
      
   private:
      I_MqttController& mMqttController;
      std::string mTopic;
      int mQos;
      
};

class MqttMessageSubscription : public MqttSubscription
{
   public:
      MqttMessageSubscription(Concurrency::I_ExecutionContext& pExecutionContext, I_MqttController& pMqttController ,const std::string& pTopic, int pQos, std::function<void(std::shared_ptr<MqttMessage>)> pCallback);
      
      void onMessage(std::shared_ptr<MqttMessage> pMessage);
      virtual void accept(I_MqttSubscriptionVisitor& pVisitor);  
   
   private:
      Concurrency::I_ExecutionContext& mExecutionContext;
      std::function<void(std::shared_ptr<MqttMessage>)> mCallback;
      
};

class MqttRawSubscription : public MqttSubscription
{
   public:
      MqttRawSubscription(I_MqttController& pMqttController, const std::string& pTopic, int pQos, std::function<void(const RawMqttMessage& pMessage)> pCallback);
      
      void onRawMessage(const RawMqttMessage& pMessage);
      virtual void accept(I_MqttSubscriptionVisitor& pVisitor);  
   
   private:
      std::function<void(const RawMqttMessage& pMessage)> mCallback;
};




} // namespace Protocols
} // namespace Bt

#endif // INC__Bt_Protocols_MqttSubscription__h
