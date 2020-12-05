//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleDescriptor
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleDescriptor__h
#define INC__Bt_Bluetooth_I_BleDescriptor__h

#include <cinttypes>
#include <cstddef>

namespace Bt {
namespace Bluetooth {

class I_BleDescriptor {
   public:
      virtual ~I_BleDescriptor() {}

      virtual bool writeValue(uint8_t* pData, size_t pLenght) = 0;
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleDescriptor__h

