//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleService
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleService__h
#define INC__Bt_Bluetooth_BleService__h

#include <host/ble_gap.h>
#include <host/ble_gatt.h>
#undef max
#undef min

#include "Bt/Bluetooth/BleClient.h"
#include "Bt/Bluetooth/I_BleClient.h"
#include "Bt/Bluetooth/I_BleService.h"
#include "Bt/Bluetooth/Utilities.h"

namespace Bt {
namespace Bluetooth {

class BleService : public I_BleService
{
   public:

      BleService(BleClient& pClient, const ble_gatt_svc& pService);
      BleService(const BleService&) = delete;
      BleService& operator=(const BleService&) = delete;
      ~BleService();

      virtual bool getCharacteristic(const BleUuid& pCharacteristicUuid, OnCharacteristicDiscover pOnCharacteristicDiscover); 
      virtual std::string toString() const;

      BleClient& client() const {return mClient;}
      BleUuid uuid() const {return toBleUuid(mService.uuid);}
      uint16_t endHandle() const {return mService.end_handle;}
      
   private:
      static int onCharacteristicDiscoverStatic(uint16_t pConnHandle, const ble_gatt_error* pError, const ble_gatt_chr* pCharacteristic, void* pArg);
      
      BleClient& mClient;
      ble_gatt_svc mService;
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleService__h

