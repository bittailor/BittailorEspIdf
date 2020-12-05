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

namespace Bt {
namespace Bluetooth {

class BleService : public I_BleService
{
   public:

      BleService(BleClient& pClient, const ble_gatt_svc& pService);
      BleService(const BleService&) = delete;
      BleService& operator=(const BleService&) = delete;
      ~BleService();

      virtual bool getCharacteristic(const BleUuid& pCharacteristicUuid); 
      virtual std::string toString() const;

      BleClient& client() const {return mClient;}

      uint16_t endHandle() const {return mService.end_handle;}

   private:
      static int onCharacteristicDiscoverStatic(uint16_t pConnHandle, const struct ble_gatt_error* pError, const ble_gatt_chr* pCharacteristic, void* pArg);
      int onCharacteristicDiscover(uint16_t pConnHandle, const struct ble_gatt_error* pError, const ble_gatt_chr* pCharacteristic);

      BleClient& mClient;
      ble_gatt_svc mService;
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleService__h

