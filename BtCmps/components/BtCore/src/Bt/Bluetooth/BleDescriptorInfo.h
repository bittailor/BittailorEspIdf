//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleDescriptorInfo
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleDescriptorInfo__h
#define INC__Bt_Bluetooth_BleDescriptorInfo__h

#include "Bt/Bluetooth/BleUuid.h"

namespace Bt {
namespace Bluetooth {

struct BleDescriptorInfo
{
   uint16_t handle;
   BleUuid uuid;    
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleDescriptorInfo__h

