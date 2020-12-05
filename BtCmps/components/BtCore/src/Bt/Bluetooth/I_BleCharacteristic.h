//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleCharacteristic
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleCharacteristic__h
#define INC__Bt_Bluetooth_I_BleCharacteristic__h

#include <functional>
#include <memory>
#include <string>
#include <map>

#include "Bt/Bluetooth/I_BleDescriptor.h"
#include "Bt/Bluetooth/BleUuid.h"

namespace Bt {
namespace Bluetooth {

class I_BleCharacteristic {
   public:
      using OnSubscribe = std::function<void()>;
      using BleDescriptor = std::shared_ptr<I_BleDescriptor>;
      using Descriptors = std::map<BleUuid,BleDescriptor>;
      using OnDescriptorsDiscover = std::function<void(const Descriptors& pDescriptors)>;

      virtual ~I_BleCharacteristic() {}

      virtual bool subscribe(OnSubscribe pOnSubscribe) = 0;
      virtual std::string toString() const = 0;
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleCharacteristic__h

