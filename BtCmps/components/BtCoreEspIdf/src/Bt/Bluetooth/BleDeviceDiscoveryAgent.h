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
#include <memory>

#include <host/ble_gap.h>
#undef max
#undef min

#include <Bt/Bluetooth/BleDeviceInfo.h>

namespace Bt {
namespace Bluetooth {

class BleDeviceDiscoveryAgent
{
   public:
      BleDeviceDiscoveryAgent(std::function<void(std::shared_ptr<BleDeviceInfo>)> pOnDiscover);
      BleDeviceDiscoveryAgent(const BleDeviceDiscoveryAgent&) = delete;
      BleDeviceDiscoveryAgent& operator=(const BleDeviceDiscoveryAgent&) = delete;
      ~BleDeviceDiscoveryAgent();

      void start(std::chrono::milliseconds pDuration);

      void interval(std::chrono::milliseconds pInterval) {mParameters.itvl = pInterval.count() / 0.625;}
      void window(std::chrono::milliseconds pWindow) {mParameters.window = pWindow.count() / 0.625;}
      void duplicateFilter(bool pEnabled) {mParameters.filter_duplicates = pEnabled;}

   private:
      static int onGapEventStatic(struct ble_gap_event* pEvent, void* pArg);
      int onGapEvent(struct ble_gap_event* pEvent);
      void onDiscover(struct ble_gap_event* pEvent);

      std::function<void(std::shared_ptr<BleDeviceInfo>)> mOnDiscover;
      uint8_t mOwnAddrType;
      struct ble_gap_disc_params mParameters; 
      
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleDeviceDiscoveryAgent__h

