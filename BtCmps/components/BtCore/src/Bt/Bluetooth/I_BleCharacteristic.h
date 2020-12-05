//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleCharacteristic
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleCharacteristic__h
#define INC__Bt_Bluetooth_I_BleCharacteristic__h

#include <functional>
#include <string>

namespace Bt {
namespace Bluetooth {

class I_BleCharacteristic {
   public:
      using OnSubscribe = std::function<void()>;


      virtual ~I_BleCharacteristic() {}

      virtual bool subscribe(OnSubscribe pOnSubscribe) = 0;
      virtual std::string toString() const = 0;
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleCharacteristic__h

