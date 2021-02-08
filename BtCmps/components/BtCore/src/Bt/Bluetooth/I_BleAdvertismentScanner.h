//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleAdvertismentScanner
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleAdvertismentScanner__h
#define INC__Bt_Bluetooth_I_BleAdvertismentScanner__h

#include <memory>

#include <Bt/Bluetooth/BleDeviceInfo.h>

namespace Bt {
namespace Bluetooth {

class I_BleAdvertismentScanner {
   public:
      class I_Listener {
         public:
            virtual ~I_Listener(){}
            virtual void onAdvertisment(std::shared_ptr<BleDeviceInfo> pDeviceInfo) = 0;   
      };

      virtual ~I_BleAdvertismentScanner() {}

      virtual void add(I_Listener& pListener) = 0;
      virtual void remove(I_Listener& pListener) = 0;
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleAdvertismentScanner__h

