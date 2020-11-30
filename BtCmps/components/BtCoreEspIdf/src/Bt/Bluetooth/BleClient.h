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



namespace Bt {
namespace Bluetooth {

class BleClient : public I_BleClient
{
   public:
      BleClient();
      BleClient(const BleClient&) = delete;
      BleClient& operator=(const BleClient&) = delete;
      ~BleClient();

      virtual bool connect(const BleAddress& pAddress);

   private:
      static int onGapEventStatic(struct ble_gap_event* pEvent, void* pArg);
      int onGapEvent(struct ble_gap_event* pEvent);
      
      BleAddress mAddress;
      std::string mAddressString;
      
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleClient__h

