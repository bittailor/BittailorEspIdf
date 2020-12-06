//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleClient
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleClient__h
#define INC__Bt_Bluetooth_BleClient__h

#include <map>

#include <host/ble_gap.h>
#undef max
#undef min

#include "Bt/Bluetooth/BleAddress.h"
#include "Bt/Bluetooth/BleUuid.h"
#include "Bt/Bluetooth/I_BleClient.h"

namespace Bt {
namespace Bluetooth {

class BleCharacteristic; 
class BleController; 
class BleService; 

class BleClient : public I_BleClient
{
   public:
      BleClient(BleController& pController, I_Listener& pListener);
      BleClient(const BleClient&) = delete;
      BleClient& operator=(const BleClient&) = delete;
      ~BleClient();

      virtual bool connect(const BleAddress& pAddress);

      virtual bool getService(const BleUuid& pServiceUuid, OnServiceDiscover pOnOnServiceDiscover);

      I_Listener& listener() const {return mListener;}
      uint16_t connectionHandle() const {return mConnectionHandle;}
      const std::string& addressString() const {return mAddressString;}
      
      I_BleClient::BleServicePtr createService(const ble_gatt_svc& pService);
      I_BleService::BleCharacteristicPtr createCharacteristic(BleService& pService, const ble_gatt_chr& pCharacteristic);

   private:

      static int onGapEventStatic(ble_gap_event* pEvent, void* pArg);
      int onGapEvent(ble_gap_event* pEvent);
      
      static int onServiceDiscoverStatic(uint16_t pConnHandle, const ble_gatt_error* pError, const ble_gatt_svc* pService, void* pArg);
      
      BleController& mController;
      I_Listener& mListener;
      BleAddress mAddress;
      std::string mAddressString;
      uint16_t mConnectionHandle;
      std::map<BleUuid,std::shared_ptr<BleService>> mServices;
      std::map<uint16_t,std::shared_ptr<BleCharacteristic>> mCharacteristics;
      
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleClient__h

