//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleUuid
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleUuid__h
#define INC__Bt_Bluetooth_BleUuid__h

#include <cstdint>
#include <vector>

namespace Bt {
namespace Bluetooth {

class BleUuid
{
   public:
      BleUuid(){}
      BleUuid(const std::vector<uint8_t>& pRaw) : mRaw(pRaw){}
      ~BleUuid(){}

      bool valid() const {return mRaw.size() ==2 || mRaw.size() ==4 || mRaw.size() ==16 ;}
      const std::vector<uint8_t> raw() const {return mRaw;} 

      bool operator ==(uint16_t pRaw) const {
         return mRaw.size() == 2 && ( pRaw == ((mRaw[0]<<8) | mRaw[1]));
      }

   private:
      std::vector<uint8_t> mRaw;
      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleUuid__h

