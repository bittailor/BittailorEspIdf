//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::DeviceFactory
//
//*************************************************************************************************

#include "Bt/Devices/Xiaomi/DeviceFactory.h"

#include <Bt/Core/Logging.h>

namespace Bt {
namespace Devices {
namespace Xiaomi {
namespace {
    constexpr const char* TAG = "Bt::Devices::Xiaomi::DeviceFactory";
}

DeviceFactory::DeviceFactory() {

}

DeviceFactory::~DeviceFactory() {

}

void DeviceFactory::registerDevice(uint16_t pDeviceTypeId, Creator pCreator) {
    mCreators.insert({pDeviceTypeId,pCreator});
}

DeviceFactory::Device DeviceFactory::createDevice(const DeviceInfo& pDeviceInfo) {
    auto iter = mCreators.find(pDeviceInfo.deviceTypeId());
    if(iter == std::end(mCreators)) {
        ESP_LOGE(TAG, "device type id 0x%04x not found", pDeviceInfo.deviceTypeId());
        return nullptr;
    }
    return iter->second(pDeviceInfo);

}

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

