//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::DeviceInfo
//
//*************************************************************************************************

#include "Bt/Devices/Xiaomi/DeviceInfo.h"

namespace Bt {
namespace Devices {
namespace Xiaomi { 
namespace {

	constexpr uint8_t cFactoryNew                   = 0b0000'0001; // 0x01
	constexpr uint8_t cConnecting                   = 0b0000'0010; // 0x02
	constexpr uint8_t cIsCentral                    = 0b0000'0100; // 0x04
	constexpr uint8_t cIsEncrypted                  = 0b0000'1000; // 0x08
	constexpr uint8_t cMacIncludeed                 = 0b0001'0000; // 0x10
	constexpr uint8_t cCapabilityIncludeed          = 0b0010'0000; // 0x20
	constexpr uint8_t cEventIncludeed               = 0b0100'0000; // 0x40
	constexpr uint8_t cManufacturerDataIncludeed    = 0b1000'0000; // 0x80

	constexpr uint8_t cManufacturerTitleIncludeed   = 0b0000'0001; // 0x01
	constexpr uint8_t cBindingCfm                   = 0b0000'0010; // 0x02
}

DeviceInfo::DeviceInfo(BleDeviceInfo pBleDeviceInfo) : mBleDeviceInfo(pBleDeviceInfo)  {
    mDeviceTypeId = (mBleDeviceInfo->serviceData()[3]<<8) | mBleDeviceInfo->serviceData()[2];
}

DeviceInfo::~DeviceInfo() {

}

bool DeviceInfo::isEncrypted() const {
    return mBleDeviceInfo->serviceData()[0] & cIsEncrypted;
}

bool DeviceInfo::isEventIncludeed() const {
    return mBleDeviceInfo->serviceData()[0] & cEventIncludeed;
}

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt

