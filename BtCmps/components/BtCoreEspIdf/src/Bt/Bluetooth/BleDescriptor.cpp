//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleDescriptor
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleDescriptor.h"

#include <Bt/Core/Logging.h>

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleDescriptor"; 
}


BleDescriptor::BleDescriptor(BleCharacteristic& pCharacteristic, BleDescriptorInfo pInfo)
: mCharacteristic(pCharacteristic), mInfo(pInfo) {

}

BleDescriptor::~BleDescriptor() {

}

bool BleDescriptor::writeValue(uint8_t* pData, size_t pLenght) {
    int rc = ble_gattc_write_no_rsp_flat(mCharacteristic.service().client().connectionHandle(), mInfo.handle, pData, pLenght);
    if(rc != ESP_OK) {
        ESP_LOGE(TAG, "writeValue failed with %d", rc); 
        return false;
    }
    return true;
}

} // namespace Bluetooth
} // namespace Bt

