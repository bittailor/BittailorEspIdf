//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleUuid
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleUuid.h"

//#include <base/rand_util.h>
//#include <base/strings/stringprintf.h>
#include <algorithm>
#include <cstring>

#include <Bt/Core/StringUtilities.h>

namespace Bt {
namespace Bluetooth {
namespace {

constexpr BleUuid cBase = BleUuid::from128BitBE(BleUuid::Uuid128Bit{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00,0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb}});

}  // namespace

using Uuid128Bit = BleUuid::Uuid128Bit;

static_assert(sizeof(BleUuid) == 16, "BleUuid must be 16 bytes long!");

const BleUuid BleUuid::cEmpty = BleUuid::from128BitBE(Uuid128Bit{{0x00}});

BleUuid::BleUuid(): mRaw{0x00} {
}

size_t BleUuid::GetShortestRepresentationSize() const {
  if (memcmp(mRaw.data() + cNumBytes32, cBase.mRaw.data() + cNumBytes32,
             cNumBytes128 - cNumBytes32) != 0) {
    return cNumBytes128;
  }
  if (mRaw[0] == 0 && mRaw[1] == 0) return cNumBytes16;
  return cNumBytes32;
}

bool BleUuid::is16Bit() const {
  return GetShortestRepresentationSize() == cNumBytes16;
}

uint16_t BleUuid::as16Bit() const { 
  return (((uint16_t)mRaw[2]) << 8) + mRaw[3]; 
}

uint32_t BleUuid::as32Bit() const {
  return (((uint32_t)mRaw[0]) << 24) + (((uint32_t)mRaw[1]) << 16) +
         (((uint32_t)mRaw[2]) << 8) + mRaw[3];
}

BleUuid BleUuid::fromString(const std::string& pUuid, bool* pIsValid) {
  if (pIsValid) *pIsValid = false;
  BleUuid ret = cBase;
  if (pUuid.empty()) return ret;
  uint8_t* p = ret.mRaw.data();
  if (pUuid.size() == cString128BitLen) {
    if (pUuid[8] != '-' || pUuid[13] != '-' || pUuid[18] != '-' ||
        pUuid[23] != '-') {
      return ret;
    }
    int c;
    int rc =
        sscanf(pUuid.c_str(),
               "%02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx"
               "-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%n",
               &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7], &p[8],
               &p[9], &p[10], &p[11], &p[12], &p[13], &p[14], &p[15], &c);
    if (rc != 16) return ret;
    if (c != cString128BitLen) return ret;
    if (pIsValid) *pIsValid = true;
  } else if (pUuid.size() == 8) {
    int c;
    int rc = sscanf(pUuid.c_str(), "%02hhx%02hhx%02hhx%02hhx%n", &p[0], &p[1],
                    &p[2], &p[3], &c);
    if (rc != 4) return ret;
    if (c != 8) return ret;
    if (pIsValid) *pIsValid = true;
  } else if (pUuid.size() == 4) {
    int c;
    int rc = sscanf(pUuid.c_str(), "%02hhx%02hhx%n", &p[2], &p[3], &c);
    if (rc != 2) return ret;
    if (c != 4) return ret;
    if (pIsValid) *pIsValid = true;
  }
  return ret;
}

BleUuid BleUuid::from16Bit(uint16_t pUuid) {
  BleUuid u = cBase;
  u.mRaw[2] = (uint8_t)((0xFF00 & pUuid) >> 8);
  u.mRaw[3] = (uint8_t)(0x00FF & pUuid);
  return u;
}

BleUuid BleUuid::from16BitLe(const uint8_t* pUuid) {
  BleUuid u = cBase;
  u.mRaw[2] = pUuid[1];
  u.mRaw[3] = pUuid[0];
  return u;
}

BleUuid BleUuid::from32Bit(uint32_t pUuid) {
  BleUuid u = cBase;
  u.mRaw[0] = (uint8_t)((0xFF000000 & pUuid) >> 24);
  u.mRaw[1] = (uint8_t)((0x00FF0000 & pUuid) >> 16);
  u.mRaw[2] = (uint8_t)((0x0000FF00 & pUuid) >> 8);
  u.mRaw[3] = (uint8_t)(0x000000FF & pUuid);
  return u;
}

BleUuid BleUuid::from32BitLe(const uint8_t* pUuid) {
  BleUuid u = cBase;
  u.mRaw[0] = pUuid[3];
  u.mRaw[1] = pUuid[2];
  u.mRaw[2] = pUuid[1];
  u.mRaw[3] = pUuid[0];
  return u;    
}

BleUuid BleUuid::from128BitBE(const uint8_t* pUuid) {
  Uuid128Bit tmp;
  memcpy(tmp.data(), pUuid, cNumBytes128);
  return from128BitBE(tmp);
}

BleUuid BleUuid::from128BitLE(const Uuid128Bit& pUuid) {
  BleUuid u;
  std::reverse_copy(pUuid.data(), pUuid.data() + cNumBytes128, u.mRaw.begin());
  return u;
}

BleUuid BleUuid::from128BitLE(const uint8_t* pUuid) {
  Uuid128Bit tmp;
  memcpy(tmp.data(), pUuid, cNumBytes128);
  return from128BitLE(tmp);
}

const Uuid128Bit BleUuid::to128BitLE() const {
  Uuid128Bit le;
  std::reverse_copy(mRaw.data(), mRaw.data() + cNumBytes128, le.begin());
  return le;
}

const Uuid128Bit& BleUuid::to128BitBE() const { 
  return mRaw; 
}

bool BleUuid::isEmpty() const { 
  return *this == cEmpty; 
}

void BleUuid::updateUuid(const BleUuid& pUuid) {
  mRaw = pUuid.mRaw;
}

bool BleUuid::operator<(const BleUuid& pRhs) const {
  return std::lexicographical_compare(mRaw.begin(), mRaw.end(), pRhs.mRaw.begin(),
                                      pRhs.mRaw.end());
}

bool BleUuid::operator==(const BleUuid& pRhs) const { 
  return mRaw == pRhs.mRaw; 
}

bool BleUuid::operator!=(const BleUuid& pRhs) const { 
  return mRaw != pRhs.mRaw; 
}

bool BleUuid::operator ==(uint16_t pRaw) const {
         return is16Bit() && ( pRaw == as16Bit());
}

std::string BleUuid::toString() const {
  return Core::stringPrintf(
      "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
      mRaw[0], mRaw[1], mRaw[2], mRaw[3], mRaw[4], mRaw[5], mRaw[6], mRaw[7], mRaw[8], mRaw[9],
      mRaw[10], mRaw[11], mRaw[12], mRaw[13], mRaw[14], mRaw[15]);
}

} // namespace Bluetooth
} // namespace Bt

