//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Protocols::I_MqttController
//
//*************************************************************************************************

#ifndef INC__Bt_Protocols_I_MqttController__h
#define INC__Bt_Protocols_I_MqttController__h

#include <functional>
#include <string>

namespace Bt {
namespace Protocols {

class MqttSubscription;

class I_MqttController {
   public:
      struct Connected{};

      virtual ~I_MqttController() {}

      virtual void publish(const char* pTopic, const std::string& pMsg, int pQos = 0, bool pRetain = false) = 0;
      virtual void add(MqttSubscription& pSubscription)=0;
      virtual void remove(MqttSubscription& pSubscription)=0;


};

} // namespace Protocols
} // namespace Bt

#endif // INC__Bt_Protocols_I_MqttController__h
