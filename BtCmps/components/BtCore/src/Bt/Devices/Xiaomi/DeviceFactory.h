//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::DeviceFactory
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_Xiaomi_DeviceFactory__h
#define INC__Bt_Devices_Xiaomi_DeviceFactory__h

#include <cstdint>
#include <functional>
#include <memory>
#include <map>

#include "Bt/Devices/Xiaomi/I_Device.h"
#include "Bt/Devices/Xiaomi/DeviceInfo.h"

namespace Bt {
namespace Devices {
namespace Xiaomi {

class DeviceFactory
{
   public:
      using Device = std::shared_ptr<I_Device>;
      using Creator = std::function<Device(const DeviceInfo&)>;
   
      DeviceFactory();
      DeviceFactory(const DeviceFactory&) = delete;
      DeviceFactory& operator=(const DeviceFactory&) = delete;
      ~DeviceFactory();

      void registerDevice(uint16_t pDeviceTypeId, Creator pCreator);
      Device createDevice(const DeviceInfo& pDeviceInfo);

   private:
      std::map<uint16_t,Creator> mCreators;

      
};

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_Xiaomi_DeviceFactory__h

