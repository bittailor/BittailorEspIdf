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
      //using On

      
      BleCharacteristic(BleService& pService, const ble_gatt_chr& pCharacteristic);
      BleCharacteristic(const BleCharacteristic&) = delete;
      BleCharacteristic& operator=(const BleCharacteristic&) = delete;
      ~BleCharacteristic();

      virtual bool subscribe(OnSubscribe pOnSubscribe);
      virtual bool writeValue(uint8_t* pData, size_t pLenght);

      BleService& service() const {return mService;}
      uint16_t valueHandle() const {return mCharacteristic.val_handle;}

      void onNotify(uint8_t* pData, size_t pLength);
      

      virtual std::string toString() const;

   private:
      bool discoverDescriptors(OnDescriptorsDiscover pOnDescriptorsDiscover);

      static int onDiscoverDescriptorsStatic(uint16_t pConnHandle, const ble_gatt_error* pError, uint16_t pChrValHandle, const ble_gatt_dsc* pDsc, void* pArg);

      BleService& mService;
      ble_gatt_chr mCharacteristic;
      OnSubscribe mOnSubscribe;
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleCharacteristic__h

