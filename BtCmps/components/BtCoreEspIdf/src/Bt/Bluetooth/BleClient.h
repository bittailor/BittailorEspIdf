//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleClient
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleClient__h
#define INC__Bt_Bluetooth_BleClient__h


#include <host/ble_gap.h>
#undef max
#undef min

#include "Bt/Bluetooth/BleAddress.h"
#include "Bt/Bluetooth/I_BleClient.h"
#include "Bt/Bluetooth/BleUuid.h"



namespace Bt {
namespace Bluetooth {

class BleClient : public I_BleClient
{
   public:
      BleClient(I_Listener& pListener);
      BleClient(const BleClient&) = delete;
      BleClient& operator=(const BleClient&) = delete;
      ~BleClient();

      virtual bool connect(const BleAddress& pAddress);

      virtual bool getService(const BleUuid& pServiceUuid);

      I_Listener& listener() const {return mListener;}
      uint16_t connectionHandle() const {return mConnectionHandle;}

   private:
      static int onGapEventStatic(struct ble_gap_event* pEvent, void* pArg);
      int onGapEvent(struct ble_gap_event* pEvent);
      
      static int onServiceDiscoverStatic(uint16_t pConnHandle, const struct ble_gatt_error* pError, const struct ble_gatt_svc* pService, void* pArg);
      int onServiceDiscover(uint16_t pConnHandle, const struct ble_gatt_error* pError, const struct ble_gatt_svc* pService);
      
      I_Listener& mListener;
      BleAddress mAddress;
      std::string mAddressString;
      uint16_t mConnectionHandle;
      
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleClient__h

