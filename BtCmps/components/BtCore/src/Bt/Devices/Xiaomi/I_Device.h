//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Devices::Xiaomi::I_Device
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_Xiaomi_I_Device__h
#define INC__Bt_Devices_Xiaomi_I_Device__h

namespace Bt {
namespace Devices {
namespace Xiaomi {

class I_Device {
   public:
      virtual ~I_Device() {}

      virtual bool connect() = 0;

      //virtual void function() = 0;
};

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_Xiaomi_I_Device__h

