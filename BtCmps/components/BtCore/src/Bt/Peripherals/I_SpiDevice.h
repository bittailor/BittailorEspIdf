//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Peripherals::I_SpiDevice
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_I_SpiDevice__h
#define INC__Bt_Peripherals_I_SpiDevice__h

#include <cstdint>
#include <vector>

namespace Bt {
namespace Peripherals {

class I_SpiDevice {
   public:
      virtual ~I_SpiDevice() {}

      virtual void send(uint8_t pData) = 0;
      virtual void send(const uint8_t* pData, size_t pLength) = 0;
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_I_SpiDevice__h
