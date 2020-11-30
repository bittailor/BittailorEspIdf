//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleController
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleController__h
#define INC__Bt_Bluetooth_BleController__h

#include <Bt/Bluetooth/I_BleController.h>

namespace Bt {
namespace Bluetooth {

class BleController : public I_BleController
{
   public:
      BleController();
      BleController(const BleController&) = delete;
      BleController& operator=(const BleController&) = delete;
      ~BleController();

      virtual std::shared_ptr<I_BleDeviceDiscoveryAgent>  createDeviceDiscoveryAgent(I_BleDeviceDiscoveryAgent::OnDiscover pOnDiscover = nullptr,I_BleDeviceDiscoveryAgent::OnDiscoverComplete pOnDiscoverComplete = nullptr);
      virtual std::shared_ptr<I_BleClient>  createClient();
      
   private:
      static void onHostResetStatic(int pReason);
      static void onHostAndControllerSyncedStatic(); 

      void onHostReset(int pReason);
      void onHostAndControllerSynced(); 
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleController__h

