//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleDeviceInfo
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleDeviceInfo.h"

namespace Bt {
namespace Bluetooth {

BleDeviceInfo::BleDeviceInfo() {

}

BleDeviceInfo::~BleDeviceInfo() {

}

void BleDeviceInfo::serviceData(const BleUuid& pServiceUuid, const uint8_t* pData, size_t pLength) {
    mServiceUuid = pServiceUuid;
    mServiceData.clear();
    mServiceData.insert(mServiceData.end(), pData, pData + pLength); 
}

} // namespace Bluetooth
} // namespace Bt

