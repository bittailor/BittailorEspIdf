//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::RgbLed
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_RgbLed__h
#define INC__Bt_Peripherals_RgbLed__h

#include <array>

#include "Bt/Peripherals/I_PwmOut.h"
#include "Bt/Peripherals/I_RgbLed.h"


namespace Bt {
namespace Peripherals {

class RgbLed : public I_RgbLed
{
   public:
      RgbLed(I_PwmOut& pRed, I_PwmOut& pGreen, I_PwmOut& pBlue, bool pCommonAnode = false);
      RgbLed(const RgbLed&) = delete;
      RgbLed& operator=(const RgbLed&) = delete;
      ~RgbLed();

      void begin();

      virtual void setHue(uint8_t pHue, uint8_t pBrightness) override;
      virtual void off() override;

   private:
      enum Color {RED=0, GREEN=1, BLUE=2};

      uint32_t value(uint32_t pValue);
      void writeOut();
      
      bool mCommonAnode;
      std::array<I_PwmOut*,3> mLeds;
      std::array<uint32_t,3> mValues;
      
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_RgbLed__h

