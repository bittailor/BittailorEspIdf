//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::HumiditySensor
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_Xiaomi_HumiditySensor__h
#define INC__Bt_Devices_Xiaomi_HumiditySensor__h

#include <cstdint>

#include <Bt/Bluetooth/I_BleController.h>

#include "Bt/Devices/Xiaomi/I_Device.h"
#include "Bt/Devices/Xiaomi/DeviceInfo.h"
#include "Bt/Devices/Xiaomi/DeviceFactory.h"


namespace Bt {
namespace Devices {
namespace Xiaomi {

class HumiditySensor : public I_Device 
{
   public:
      static constexpr uint16_t cDeviceTypeId = 0x055B;
      using BleClient = std::shared_ptr<Bt::Bluetooth::I_BleClient>;
      
      static void registerAtFactory(DeviceFactory& pFactory, Bt::Bluetooth::I_BleController& pBleController);

      HumiditySensor(Bt::Bluetooth::I_BleController& pBleController, const DeviceInfo& pDeviceInfo);
      HumiditySensor(const HumiditySensor&) = delete;
      HumiditySensor& operator=(const HumiditySensor&) = delete;
      ~HumiditySensor();

      virtual bool connect();

   private:
      BleClient mBleClient;
      Bt::Bluetooth::BleAddress mAddress;   
      
};

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_Xiaomi_HumiditySensor__h

