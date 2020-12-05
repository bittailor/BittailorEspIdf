//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleDeviceDiscoveryAgent
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleDeviceDiscoveryAgent__h
#define INC__Bt_Bluetooth_BleDeviceDiscoveryAgent__h

#include <cstdint>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include <host/ble_gap.h>
#undef max
#undef min

#include "Bt/Bluetooth/BleDeviceInfo.h"
#include "Bt/Bluetooth/I_BleDeviceDiscoveryAgent.h"

namespace Bt {
namespace Bluetooth {

class BleDeviceDiscoveryAgent : public I_BleDeviceDiscoveryAgent
{
   public:
    
      BleDeviceDiscoveryAgent(OnDiscover pOnDiscover = nullptr , OnDiscoverComplete pOnDiscoverComplete = nullptr);
      BleDeviceDiscoveryAgent(const BleDeviceDiscoveryAgent&) = delete;
      BleDeviceDiscoveryAgent& operator=(const BleDeviceDiscoveryAgent&) = delete;
      ~BleDeviceDiscoveryAgent();

      virtual void start(std::chrono::milliseconds pDuration);

      virtual void onDiscoverCallback(OnDiscover pOnDiscover) {mOnDiscover = pOnDiscover; }
      virtual void OnDiscoverCompleteCallback(OnDiscoverComplete pOnDiscoverComplete) {mOnDiscoverComplete = pOnDiscoverComplete; }
     
      virtual void interval(std::chrono::milliseconds pInterval) {mParameters.itvl = pInterval.count() / 0.625;}
      virtual void window(std::chrono::milliseconds pWindow) {mParameters.window = pWindow.count() / 0.625;}
      virtual void duplicateFilter(bool pEnabled) {mParameters.filter_duplicates = pEnabled;}

   private:
      static int onGapEventStatic(ble_gap_event* pEvent, void* pArg);
      int onGapEvent(ble_gap_event* pEvent);
      void onDiscover(ble_gap_event* pEvent);
      void onDiscoverComplete(ble_gap_event* pEvent); 

      OnDiscover mOnDiscover;
      OnDiscoverComplete mOnDiscoverComplete;
      std::map<BleAddress,std::shared_ptr<BleDeviceInfo>> mDiscoveredDevices;
      ble_gap_disc_params mParameters; 
      uint8_t mOwnAddrType;
      
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleDeviceDiscoveryAgent__h

