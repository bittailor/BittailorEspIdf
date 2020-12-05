//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleService
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleService__h
#define INC__Bt_Bluetooth_I_BleService__h

#include <memory>
#include <string>

#include "Bt/Bluetooth/BleUuid.h"
#include "Bt/Bluetooth/I_BleCharacteristic.h"

namespace Bt {
namespace Bluetooth {

class I_BleService {
   public:
      using BleCharacteristic = std::shared_ptr<I_BleCharacteristic>;
      
      using OnCharacteristicDiscover = std::function<void(BleCharacteristic pCharacteristic)>;
     
      virtual ~I_BleService() {}
      virtual  bool getCharacteristic(const BleUuid& pCharacteristicUuid, OnCharacteristicDiscover pOnCharacteristicDiscover) = 0; 
      virtual std::string toString() const = 0;
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleService__h

