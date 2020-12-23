//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::System::System
//
//*************************************************************************************************

#include "Bt/System/System.h"

#include <esp_system.h>

#include <Bt/Core/StringUtilities.h>

namespace Bt {
namespace System {

System::System() {
    uint8_t mac[6]; 
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    mId = Core::stringPrintf("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

System::~System() {

}

} // namespace System
} // namespace Bt

