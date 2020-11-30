//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleClient
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleClient__h
#define INC__Bt_Bluetooth_I_BleClient__h

#include "Bt/Bluetooth/BleAddress.h"

namespace Bt {
namespace Bluetooth {

class I_BleClient {
   public:
      virtual ~I_BleClient() {}

      virtual bool connect(const BleAddress& pAddress) = 0;
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleClient__h

