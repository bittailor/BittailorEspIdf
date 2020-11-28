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

      static BleAddress from48BitLe(const uint8_t* pAddress);
      static BleAddress from48BitLe(const Address48Bit& pAddress); 

      bool isEmpty() const;

      bool operator<(const BleAddress& pRhs) const;
      bool operator==(const BleAddress& pRhs) const;
      bool operator!=(const BleAddress& pRhs) const;

      std::string toString() const;

   private:
      Address48Bit mRaw;
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleAddress__h

