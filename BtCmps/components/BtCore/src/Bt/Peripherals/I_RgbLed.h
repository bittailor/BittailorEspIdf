//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Peripherals::I_RgbLed
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_I_RgbLed__h
#define INC__Bt_Peripherals_I_RgbLed__h

#include <cstdint>

namespace Bt {
namespace Peripherals {

class I_RgbLed {
   public:
      virtual ~I_RgbLed() {}

      virtual void setHue(uint8_t pHue, uint8_t pBrightness) = 0;
      virtual void off() = 0;
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_I_RgbLed__h

