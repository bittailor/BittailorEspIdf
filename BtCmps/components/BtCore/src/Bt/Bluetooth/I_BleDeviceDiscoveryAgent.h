//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Bluetooth::I_BleDeviceDiscoveryAgent
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_I_BleDeviceDiscoveryAgent__h
#define INC__Bt_Bluetooth_I_BleDeviceDiscoveryAgent__h

#include <chrono>
#include <functional>
#include <vector>

#include "Bt/Bluetooth/BleDeviceInfo.h"

namespace Bt {
namespace Bluetooth {

class I_BleDeviceDiscoveryAgent {
   public:
      typedef std::function<void(std::shared_ptr<BleDeviceInfo>)> OnDiscover;
      typedef std::function<void(const std::vector<std::shared_ptr<BleDeviceInfo>>&)> OnDiscoverComplete;

      virtual ~I_BleDeviceDiscoveryAgent() {}

      virtual void start(std::chrono::milliseconds pDuration) = 0;

      virtual void onDiscoverCallback(OnDiscover pOnDiscover) = 0;
      virtual void OnDiscoverCompleteCallback(OnDiscoverComplete pOnDiscoverComplete) = 0;
     
      virtual void interval(std::chrono::milliseconds pInterval) = 0;
      virtual void window(std::chrono::milliseconds pWindow) = 0;
      virtual void duplicateFilter(bool pEnabled) = 0;

      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_I_BleDeviceDiscoveryAgent__h

