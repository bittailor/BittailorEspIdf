//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleController
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleController__h
#define INC__Bt_Bluetooth_I_BleController__h

namespace Bt {
namespace Bluetooth {

class I_BleController {
   public:
      struct Synced{};
      
      virtual ~I_BleController() {}

      //virtual void function() = 0;
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleController__h

