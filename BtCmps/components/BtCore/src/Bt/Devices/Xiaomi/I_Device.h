//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Devices::Xiaomi::I_Device
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_Xiaomi_I_Device__h
#define INC__Bt_Devices_Xiaomi_I_Device__h

#include <functional>
#include <map>
#include <string>


namespace Bt {
namespace Devices {
namespace Xiaomi {

class I_Device {
   public:
      using Values = std::map<std::string,float>;
      using OnReading = std::function<void(const std::string& pId, const Values& pValues)>;

      virtual ~I_Device() {}
      virtual bool connect(OnReading pOnReading) = 0;
};

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_Xiaomi_I_Device__h

