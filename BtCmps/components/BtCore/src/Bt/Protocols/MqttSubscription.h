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

class MqttSubscription
{
   public:
      MqttSubscription(Concurrency::I_ExecutionContext& pExecutionContext, I_MqttController& pMqttController, std::function<void(std::shared_ptr<MqttMessage>)> pCallback ,const std::string& pTopic, int pQos);
      MqttSubscription(const MqttSubscription&) = delete;
      MqttSubscription& operator=(const MqttSubscription&) = delete;
      ~MqttSubscription();

      const std::string& topic() const {return mTopic;}
      int qos() const {return mQos;}

      void onMessage(std::shared_ptr<MqttMessage> pMessage);

   private:
      Concurrency::I_ExecutionContext& mExecutionContext;
      I_MqttController& mMqttController;
      std::function<void(std::shared_ptr<MqttMessage>)> mCallback;
      std::string mTopic;
      int mQos;
      
};

} // namespace Protocols
} // namespace Bt

#endif // INC__Bt_Protocols_MqttSubscription__h
