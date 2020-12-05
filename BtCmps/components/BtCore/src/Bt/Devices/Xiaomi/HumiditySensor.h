//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::HumiditySensor
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_Xiaomi_HumiditySensor__h
#define INC__Bt_Devices_Xiaomi_HumiditySensor__h

#include <cstdint>

#include <Bt/Bluetooth/I_BleController.h>
#include <Bt/Concurrency/I_ExecutionContext.h>

#include "Bt/Devices/Xiaomi/I_Device.h"
#include "Bt/Devices/Xiaomi/DeviceInfo.h"
#include "Bt/Devices/Xiaomi/DeviceFactory.h"


namespace Bt {
namespace Devices {
namespace Xiaomi {

class HumiditySensor : public I_Device, private Bt::Bluetooth::I_BleClient::I_Listener 
{
   public:
      static constexpr uint16_t cDeviceTypeId = 0x055B;
      using BleClient = std::shared_ptr<Bt::Bluetooth::I_BleClient>;
      
      static void registerAtFactory(Concurrency::I_ExecutionContext& pExecutionContext, DeviceFactory& pFactory, Bt::Bluetooth::I_BleController& pBleController);

      HumiditySensor(Concurrency::I_ExecutionContext& pExecutionContext, Bt::Bluetooth::I_BleController& pBleController, const DeviceInfo& pDeviceInfo);
      HumiditySensor(Concurrency::I_ExecutionContext& pExecutionContext, Bt::Bluetooth::I_BleController& pBleController, const Bluetooth::BleAddress& pBleAddress);
      HumiditySensor(const HumiditySensor&) = delete;
      HumiditySensor& operator=(const HumiditySensor&) = delete;
      ~HumiditySensor();

      virtual bool connect();

   private:
      virtual void onConnect();
      virtual void onDisconnect();
      virtual void onServiceDiscover(BleService pService);
      virtual void onCharacteristicDiscover(BleCharacteristic pCharacteristic);

      Concurrency::I_ExecutionContext& mExecutionContext;
      BleClient mBleClient;
      Bt::Bluetooth::BleAddress mAddress;
      BleService mService;  
      BleCharacteristic mTemperatureAndHumidityCharacteristic; 
      
};

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_Xiaomi_HumiditySensor__h
