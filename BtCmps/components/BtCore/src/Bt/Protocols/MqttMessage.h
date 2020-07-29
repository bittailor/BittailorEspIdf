//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Protocols::MqttMessage
//
//*************************************************************************************************

#ifndef INC__Bt_Protocols_MqttMessage__h
#define INC__Bt_Protocols_MqttMessage__h

#include <string>

namespace Bt {
namespace Protocols {

class MqttMessage {
   public:
      MqttMessage(const char* pTopic, size_t pTopicLength, const char* pData, size_t pDataLength)
      : topic{pTopic, pTopicLength}, data{pData, pDataLength} {}
      const std::string topic;
      const std::string data;
};

} // namespace Protocols
} // namespace Bt

#endif // INC__Bt_Protocols_MqttMessage__h
