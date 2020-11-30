//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleAddress
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleAddress__h
#define INC__Bt_Bluetooth_BleAddress__h

#include <array>

namespace Bt {
namespace Bluetooth {

class BleAddress
{
   public:
      static constexpr size_t cNumBytes = 6;

      static const BleAddress cEmpty;  // 00:00:00:00:00:00

      using Address48Bit = std::array<uint8_t, cNumBytes>;
  
      BleAddress();
      ~BleAddress();

      static BleAddress from48BitLe(const uint8_t* pAddress, uint8_t  pType = 0);
      static BleAddress from48BitLe(const Address48Bit& pAddress, uint8_t  pType = 0); 

      void to48BitLe(uint8_t pAddress[], uint8_t* pType = nullptr) const;
      template<typename T> void to(T& pT) const { to48BitLe(pT.val, &pT.type);} 

      bool isEmpty() const;

      bool operator<(const BleAddress& pRhs) const;
      bool operator==(const BleAddress& pRhs) const;
      bool operator!=(const BleAddress& pRhs) const;

      std::string toString() const;

   private:
      uint8_t  mType;
      Address48Bit mRaw;
      
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleAddress__h

