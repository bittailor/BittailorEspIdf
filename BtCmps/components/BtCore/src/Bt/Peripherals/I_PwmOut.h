//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Peripherals::I_PwmOut
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_I_PwmOut__h
#define INC__Bt_Peripherals_I_PwmOut__h

#include <stdint.h>

namespace Bt {
namespace Peripherals {

class I_PwmOut {
   public:
      virtual ~I_PwmOut() {}

      virtual void configure(double pFrequency, uint8_t pDutyResolutionInBits) = 0;
      virtual void write(uint32_t pDuty) = 0;
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_I_PwmOut__h

