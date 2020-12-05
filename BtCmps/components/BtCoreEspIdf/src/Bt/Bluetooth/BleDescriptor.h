//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleDescriptor
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleDescriptor__h
#define INC__Bt_Bluetooth_BleDescriptor__h

#include "Bt/Bluetooth/I_BleDescriptor.h"
#include "Bt/Bluetooth/BleDescriptorInfo.h"
#include "Bt/Bluetooth/BleCharacteristic.h"

namespace Bt {
namespace Bluetooth {

class BleDescriptor : public I_BleDescriptor
{
   public:
      BleDescriptor(BleCharacteristic& pCharacteristic, BleDescriptorInfo pInfo);
      BleDescriptor(const BleDescriptor&) = delete;
      BleDescriptor& operator=(const BleDescriptor&) = delete;
      ~BleDescriptor();

      virtual bool writeValue(uint8_t* pData, size_t pLenght);

   private:
      BleCharacteristic& mCharacteristic;
      BleDescriptorInfo mInfo;

      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleDescriptor__h

