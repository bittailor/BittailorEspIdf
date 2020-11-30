//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::DeviceInfo
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_Xiaomi_DeviceInfo__h
#define INC__Bt_Devices_Xiaomi_DeviceInfo__h

#include <memory>

#include <Bt/Bluetooth/BleDeviceInfo.h>

namespace Bt {
namespace Devices {
namespace Xiaomi {

class DeviceInfo
{
   public:
      static constexpr uint16_t cLYWSD03MMC = 0x055B;
          
      using BleDeviceInfo = std::shared_ptr<Bt::Bluetooth::BleDeviceInfo>; 

      DeviceInfo(BleDeviceInfo pBleDeviceInfo);
      DeviceInfo(const DeviceInfo&) = delete;
      DeviceInfo& operator=(const DeviceInfo&) = delete;
      ~DeviceInfo();

      bool isEncrypted() const;
      bool isEventIncludeed() const;

      uint16_t deviceTypeId() const {return mDeviceTypeId;}
      std::shared_ptr<Bt::Bluetooth::BleDeviceInfo> bleDeviceInfo() const {return mBleDeviceInfo;} 

   private:
      BleDeviceInfo mBleDeviceInfo;
      uint16_t mDeviceTypeId;
      
};

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_Xiaomi_DeviceInfo__h

