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
      MqttMessage(const char* pTopic, size_t pTopicLength, const char* pData, size_t pDataLength, size_t pCurrentDataOffset, size_t pTotalDataLength)
      : topic{pTopic, pTopicLength}, data{pData, pDataLength}, currentDataOffset{pCurrentDataOffset}, totalDataLength(pTotalDataLength){}
      
      const std::string topic;
      const std::string data;
      const size_t currentDataOffset;
      const size_t totalDataLength;
 
};

struct RawMqttMessage {
   const char* topic;
   size_t topicLength;
   const char* data;
   size_t dataLength;
   size_t currentDataOffset;
   size_t totalDataLength;   
};

} // namespace Protocols
} // namespace Bt

#endif // INC__Bt_Protocols_MqttMessage__h
