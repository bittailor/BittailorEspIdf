//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::Utilities
//
//*************************************************************************************************

#include "Bt/Bluetooth/Utilities.h"

namespace Bt {
namespace Bluetooth {

BleUuid toBleUuid(const ble_uuid_any_t& pNativeUuid) {
    switch (pNativeUuid.u.type) {
        case BLE_UUID_TYPE_16:  return BleUuid::from16Bit(pNativeUuid.u16.value);
        case BLE_UUID_TYPE_32:  return BleUuid::from32Bit(pNativeUuid.u32.value);
        case BLE_UUID_TYPE_128: return BleUuid::from128BitLE(pNativeUuid.u128.value);
    }    
    return BleUuid::cEmpty; 
}

} // namespace Bluetooth
} // namespace Bt

