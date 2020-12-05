//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleAddress
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleAddress.h"

#include <algorithm>

#include <Bt/Core/StringUtilities.h>

namespace Bt {
namespace Bluetooth {

static_assert(sizeof(BleAddress) == 7, "BleAddress must be 6 bytes long!");

const BleAddress BleAddress::cEmpty = BleAddress::from48BitLe(BleAddress::Address48Bit{{0x00}});

BleAddress::BleAddress() : mRaw{0x00} {
}

BleAddress::~BleAddress() {
}

BleAddress BleAddress::from48BitLe(const uint8_t* pAddress, uint8_t  pType) {
  BleAddress address;
  address.mType = pType;
  address.mRaw[0] = pAddress[5];
  address.mRaw[1] = pAddress[4];
  address.mRaw[2] = pAddress[3];
  address.mRaw[3] = pAddress[2];
  address.mRaw[4] = pAddress[1];
  address.mRaw[5] = pAddress[0];
  return address;    
}

BleAddress BleAddress::from48BitLe(const Address48Bit& pAddress, uint8_t  pType) {
  BleAddress address;
  address.mType = pType;
  std::reverse_copy(pAddress.data(), pAddress.data() + cNumBytes, address.mRaw.begin());
  return address;
}

BleAddress BleAddress::from48BitBe(const uint8_t* pAddress, uint8_t  pType){
  BleAddress address;
  address.mType = pType;
  std::copy(pAddress, pAddress + cNumBytes, address.mRaw.begin()); 
  return address;     
}

BleAddress BleAddress::from48BitBe(const Address48Bit& pAddress, uint8_t  pType) {
  BleAddress address;
  address.mType = pType;
  address.mRaw = pAddress; 
  return address;     
}

void BleAddress::to48BitLe(uint8_t pAddress[], uint8_t* pType) const {
  pAddress[5] = mRaw[0];
  pAddress[4] = mRaw[1];
  pAddress[3] = mRaw[2];
  pAddress[2] = mRaw[3];
  pAddress[1] = mRaw[4];
  pAddress[0] = mRaw[5];  
  if(pType != nullptr) {
    *pType = mType;
  }
}



bool BleAddress::isEmpty() const { 
  return *this == cEmpty; 
}

bool BleAddress::operator<(const BleAddress& pRhs) const {
  return std::lexicographical_compare(mRaw.begin(), mRaw.end(), pRhs.mRaw.begin(), pRhs.mRaw.end());
}

bool BleAddress::operator==(const BleAddress& pRhs) const { 
  return mRaw == pRhs.mRaw; 
}

bool BleAddress::operator!=(const BleAddress& pRhs) const { 
  return mRaw != pRhs.mRaw; 
}

std::string BleAddress::toString() const {
  return Core::stringPrintf(
      "%02X:%02X:%02X:%02X:%02X:%02X",
      mRaw[0], mRaw[1], mRaw[2], mRaw[3], mRaw[4], mRaw[5]);
}

} // namespace Bluetooth
} // namespace Bt

