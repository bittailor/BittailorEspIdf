//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::Utilities
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_Utilities__h
#define INC__Bt_Bluetooth_Utilities__h

#include <host/ble_uuid.h>

#include "Bt/Bluetooth/BleUuid.h"

namespace Bt {
namespace Bluetooth {

   BleUuid toBleUuid(const ble_uuid_any_t& pNativeUuid);

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_Utilities__h

