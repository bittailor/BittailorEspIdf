//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleCharacteristic
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleCharacteristic__h
#define INC__Bt_Bluetooth_BleCharacteristic__h

#include <host/ble_gap.h>
#include <host/ble_gatt.h>
#undef max
#undef min

#include "Bt/Bluetooth/I_BleCharacteristic.h"
#include "Bt/Bluetooth/BleService.h"

namespace Bt {
namespace Bluetooth {

class BleCharacteristic : public I_BleCharacteristic
{
   public:
      
      BleCharacteristic(BleService& pService, const ble_gatt_chr& pCharacteristic);
      BleCharacteristic(const BleCharacteristic&) = delete;
      BleCharacteristic& operator=(const BleCharacteristic&) = delete;
      ~BleCharacteristic();

      virtual bool subscribe(OnSubscribe pOnSubscribe);



      virtual std::string toString() const;

   private:
      bool discoverDescriptors();

      static int onDiscoverDescriptorsStatic(uint16_t pConnHandle, const ble_gatt_error* pError, uint16_t pChrValHandle, const ble_gatt_dsc* pDsc, void* pArg);
      int onDiscoverDescriptors(uint16_t pConnHandle, const ble_gatt_error* pError, uint16_t pChrValHandle, const ble_gatt_dsc* pDsc);


      BleService& mService;
      ble_gatt_chr mCharacteristic;
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleCharacteristic__h

