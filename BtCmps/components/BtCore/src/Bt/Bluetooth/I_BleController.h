//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleController
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleController__h
#define INC__Bt_Bluetooth_I_BleController__h

#include <memory>

#include "Bt/Bluetooth/I_BleDeviceDiscoveryAgent.h"
#include "Bt/Bluetooth/I_BleClient.h"

namespace Bt {
namespace Bluetooth {

class I_BleController {
   public:
      struct Synced{};
      virtual ~I_BleController() {}
      virtual std::shared_ptr<I_BleDeviceDiscoveryAgent>  createDeviceDiscoveryAgent(I_BleDeviceDiscoveryAgent::OnDiscover pOnDiscover = nullptr,I_BleDeviceDiscoveryAgent::OnDiscoverComplete pOnDiscoverComplete = nullptr) = 0;
      virtual std::shared_ptr<I_BleClient>  createClient(I_BleClient::I_Listener& pI_Listener) = 0;
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleController__h

