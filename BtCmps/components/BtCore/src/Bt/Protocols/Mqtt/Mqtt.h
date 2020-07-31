//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::Mqtt::Mqtt
//
//*************************************************************************************************

#ifndef INC__Bt_Protocols_Mqtt_Mqtt__h
#define INC__Bt_Protocols_Mqtt_Mqtt__h

#include <string>
#include <vector>

namespace Bt {
namespace Protocols {
namespace Mqtt {

constexpr const char TOPIC_SEPARATOR = '/';

std::vector<std::string> split(const std::string& pTopic);

std::string join(const std::vector<std::string>& pParts);

bool topicMatchesSubscription(const std::string& pSubscription, const std::string& pTopic);



} // namespace Mqtt
} // namespace Protocols
} // namespace Bt

#endif // INC__Bt_Protocols_Mqtt_Mqtt__h
