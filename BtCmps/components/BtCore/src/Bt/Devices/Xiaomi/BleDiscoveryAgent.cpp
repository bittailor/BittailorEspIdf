//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::BleDiscoveryAgent
//
//*************************************************************************************************

#include "Bt/Devices/Xiaomi/BleDiscoveryAgent.h"

#include <Bt/Core/Logging.h>

namespace Bt {
namespace Devices {
namespace Xiaomi {
namespace {
    constexpr const char* TAG = "Bt::Devices::Xiaomi::BleDiscoveryAgent";

    constexpr uint16_t XiaomiUuid = 0xfe95; // https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf

}



BleDiscoveryAgent::BleDiscoveryAgent(BleController& pBleController) 
: mBleController(pBleController), mDiscoveryAgent(nullptr) {
}

BleDiscoveryAgent::~BleDiscoveryAgent() {

}

bool BleDiscoveryAgent::discoverBleDevices(std::chrono::milliseconds pDuration, OnDiscovered pOnDiscovered) {
    if(mDiscoveryAgent != nullptr) {
        ESP_LOGW(TAG, "already a discovery active");
        return false;
    }
    mDiscoveryAgent = mBleController.createDeviceDiscoveryAgent(
        [](auto pDevice){   
        },
        [this, pOnDiscovered](auto&& pDevices){
            std::vector<BleDeviceInfo> xiaomiDevices;   
            for (auto device : pDevices)
            {
                if(device->serviceUuid() == XiaomiUuid) {
                    xiaomiDevices.push_back(device);
                }
            }
            pOnDiscovered(xiaomiDevices);
            mDiscoveryAgent = nullptr;
        }
    );
    mDiscoveryAgent->interval(std::chrono::milliseconds(35));
    mDiscoveryAgent->window(std::chrono::milliseconds(20));
    mDiscoveryAgent->duplicateFilter(true);

    mDiscoveryAgent->start(pDuration);
    return true;
}

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

