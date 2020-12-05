//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleUuid
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleUuid__h
#define INC__Bt_Bluetooth_BleUuid__h

#include <stdint.h>
#include <array>
#include <string>
#include <vector>

namespace Bt {
namespace Bluetooth {

class BleUuid final {
 public:
  static constexpr size_t cNumBytes128 = 16;
  static constexpr size_t cNumBytes32 = 4;
  static constexpr size_t cNumBytes16 = 2;
  static constexpr size_t cString128BitLen = 36;
  
  static const BleUuid cEmpty;  // 00000000-0000-0000-0000-000000000000
  
  using Uuid128Bit = std::array<uint8_t, cNumBytes128>;
  
  BleUuid();
  
  size_t GetShortestRepresentationSize() const;
  
  bool is16Bit() const;
  
  uint16_t as16Bit() const;
  
  uint32_t as32Bit() const;
  
  static BleUuid fromString(const std::string& pUuid, bool* pIsValid = nullptr);
  
  static BleUuid from16Bit(uint16_t pUuid);
  static BleUuid from16BitLe(const uint8_t* pUuid); 

  static BleUuid from32Bit(uint32_t pUuid);
  static BleUuid from32BitLe(const uint8_t* pUuid); 
  
  static BleUuid from128BitBE(const uint8_t* pUuid);
  static constexpr BleUuid from128BitBE(const Uuid128Bit& pUuid) {
    BleUuid uuid(pUuid);
    return uuid;
  }
  static BleUuid from128BitLE(const Uuid128Bit& pUuid);
  static BleUuid from128BitLE(const uint8_t* pUuid);
  void to128BitLE(uint8_t* pRaw) const;
  
  std::string toString() const;
  
  bool isEmpty() const;
  
  void updateUuid(const BleUuid& pUuid);

  bool operator<(const BleUuid& pRhs) const;
  bool operator==(const BleUuid& pRhs) const;
  bool operator!=(const BleUuid& pRhs) const;

  bool operator ==(uint16_t pRaw) const;

 private:
  constexpr BleUuid(const Uuid128Bit& pRaw) : mRaw{pRaw} {};
  // Network-byte-ordered ID (Big Endian).
  Uuid128Bit mRaw;
};

} // namespace Bluetooth
} // namespace Bt

/*
inline std::ostream& operator<<(std::ostream& os, const Bt::Bluetooth::::BleUuid& a) {
  os << a.toString();
  return os;
}
// Custom std::hash specialization so that bluetooth::UUID can be used as a key
// in std::unordered_map.
namespace std {
template <>
struct hash<Bt::Bluetooth::BleUuid> {
  std::size_t operator()(const Bt::Bluetooth::BleUuid& key) const {
    const auto& uuid_bytes = key.To128BitBE();
    std::hash<std::string> hash_fn;
    return hash_fn(std::string(reinterpret_cast<const char*>(uuid_bytes.data()),
                               uuid_bytes.size()));
  }
};
*/

#endif // INC__Bt_Bluetooth_BleUuid__h

