//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleClient
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleClient__h
#define INC__Bt_Bluetooth_I_BleClient__h

#include <memory>

#include "Bt/Bluetooth/BleAddress.h"
#include "Bt/Bluetooth/BleUuid.h"
#include "Bt/Bluetooth/I_BleService.h"

namespace Bt {
namespace Bluetooth {

class I_BleClient {
   public:
      using BleService = std::shared_ptr<I_BleService>;
      using BleCharacteristic = std::shared_ptr<I_BleCharacteristic>;
      class I_Listener{
         public:
            using BleService = I_BleClient::BleService;
            using BleCharacteristic = std::shared_ptr<I_BleCharacteristic>;
            virtual ~I_Listener() {}
            virtual void onConnect() = 0;
            virtual void onDisconnect() = 0;
            virtual void onServiceDiscover(BleService pService) = 0;
            virtual void onCharacteristicDiscover(BleCharacteristic pCharacteristic) = 0;
      };
      
      virtual ~I_BleClient() {}

      virtual bool connect(const BleAddress& pAddress) = 0;
      virtual bool getService(const BleUuid& pServiceUuid) = 0;
};

} // namespace Bluetoosth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleClient__h

