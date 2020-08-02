//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Peripherals::I_PwmTimer
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_I_PwmTimer__h
#define INC__Bt_Peripherals_I_PwmTimer__h

#include <cstdint>

namespace Bt {
namespace Peripherals {

class I_PwmTimer {
   public:
      virtual ~I_PwmTimer() {}

      virtual void configure(uint32_t pFrequency) = 0;
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_I_PwmTimer__h
