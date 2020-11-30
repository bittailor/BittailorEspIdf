//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::HumiditySensor
//
//*************************************************************************************************

#include "Bt/Devices/Xiaomi/HumiditySensor.h"

#include <Bt/Core/Logging.h>

namespace Bt {
namespace Devices {
namespace Xiaomi {
namespace {
    constexpr const char* TAG = "Bt::Devices::Xiaomi::BleDiscoveryAgent";
}

void HumiditySensor::registerAtFactory(DeviceFactory& pFactory, Bt::Bluetooth::I_BleController& pBleController) {
    pFactory.registerDevice(HumiditySensor::cDeviceTypeId,[&pBleController](auto&& pDeviceInfo){
        return std::make_shared<HumiditySensor>(pBleController, pDeviceInfo);
    });   
}

HumiditySensor::HumiditySensor(Bt::Bluetooth::I_BleController& pBleController, const DeviceInfo& pDeviceInfo)
: mBleClient(pBleController.createClient()), mAddress(pDeviceInfo.bleDeviceInfo()->address()) {
}

HumiditySensor::~HumiditySensor() {
}

bool HumiditySensor::connect() {
    ESP_LOGI(TAG, "connect to => %s", mAddress.toString().c_str()); 
    return mBleClient->connect(mAddress);
}

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

