//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleDeviceInfo
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleDeviceInfo__h
#define INC__Bt_Bluetooth_BleDeviceInfo__h

#include <array>
#include <algorithm>

#include <Bt/Bluetooth/BleUuid.h>

namespace Bt {
namespace Bluetooth {

typedef std::array<uint8_t,6> BleAddress;

class BleDeviceInfo
{
   public:
      

      BleDeviceInfo();
      BleDeviceInfo(const BleDeviceInfo&) = delete;
      BleDeviceInfo& operator=(const BleDeviceInfo&) = delete;
      ~BleDeviceInfo();

      void address(const BleAddress& pAddress) {mAddress = pAddress;}
      void address(const uint8_t* pAddress) {std::copy_n(pAddress, mAddress.size(), std::begin(mAddress));}
      const BleAddress& address() const {return mAddress;}

      const BleUuid& serviceUuid() const {return mServiceUuid;}
      const std::vector<uint8_t>& serviceData() const {return mServiceData;}

      void serviceData(const BleUuid& pServiceUuid, const uint8_t* pData, size_t pLength);

   private:
      BleAddress mAddress;
      BleUuid mServiceUuid;
      std::vector<uint8_t> mServiceData;
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleDeviceInfo__h

