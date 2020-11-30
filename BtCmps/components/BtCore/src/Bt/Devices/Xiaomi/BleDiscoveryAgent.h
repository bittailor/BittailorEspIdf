//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::BleDiscoveryAgent
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_Xiaomi_BleDiscoveryAgent__h
#define INC__Bt_Devices_Xiaomi_BleDiscoveryAgent__h

#include <Bt/Bluetooth/I_BleController.h>
#include <Bt/Bluetooth/I_BleDeviceDiscoveryAgent.h>

namespace Bt {
namespace Devices {
namespace Xiaomi {

class BleDiscoveryAgent
{
   public:
      using BleController = Bt::Bluetooth::I_BleController;
      using BleDeviceDiscoveryAgent = Bt::Bluetooth::I_BleDeviceDiscoveryAgent;
      using BleDeviceInfo = std::shared_ptr<Bt::Bluetooth::BleDeviceInfo>;
      typedef std::function<void(const std::vector<BleDeviceInfo>&)> OnDiscovered;

      BleDiscoveryAgent(BleController& pBleController);
      BleDiscoveryAgent(const BleDiscoveryAgent&) = delete;
      BleDiscoveryAgent& operator=(const BleDiscoveryAgent&) = delete;
      ~BleDiscoveryAgent();

      bool discoverBleDevices(std::chrono::milliseconds pDuration, OnDiscovered pOnDiscovered);

   private:
      BleController& mBleController;
      std::shared_ptr<BleDeviceDiscoveryAgent> mDiscoveryAgent;  
};

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_Xiaomi_BleDiscoveryAgent__h

