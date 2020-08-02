//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::RgbLed
//
//*************************************************************************************************

#include "Bt/Peripherals/RgbLed.h"

#include "Bt/Peripherals/Tag.h"

namespace Bt {
namespace Peripherals {

RgbLed::RgbLed(I_PwmOut& pRed, I_PwmOut& pGreen, I_PwmOut& pBlue, bool pCommonAnode)
: mCommonAnode(pCommonAnode), mLeds{&pRed, &pGreen, &pBlue}, mValues{value(0), value(0) ,value(0)}  {
   off();
}

RgbLed::~RgbLed() {
}

void RgbLed::setHue(uint8_t pHue, uint8_t pBrightness) {
   uint16_t scaledHue = (pHue * 6);
   uint8_t segment = scaledHue / 256;  
   uint16_t segmentOffset = scaledHue - (segment * 256);

   uint8_t complement = 0;
   uint16_t prev = (pBrightness * (255 - segmentOffset)) / 256;
   uint16_t next = (pBrightness * segmentOffset) / 256;

   if (mCommonAnode) {
      pBrightness = 255 - pBrightness;
      complement = 255;
      prev = 255 - prev;
      next = 255 - next;
   }

   switch (segment) {
      case 0:  // red
         mValues[0] = pBrightness;
         mValues[1] = next;
         mValues[2] = complement;
         break;
      case 1:  // yellow
         mValues[0] = prev;
         mValues[1] = pBrightness;
         mValues[2] = complement;
         break;
      case 2:  // green
         mValues[0] = complement;
         mValues[1] = pBrightness;
         mValues[2] = next;
         break;
      case 3:  // cyan
         mValues[0] = complement;
         mValues[1] = prev;
         mValues[2] = pBrightness;
         break;
      case 4:  // blue
         mValues[0] = next;
         mValues[1] = complement;
         mValues[2] = pBrightness;
         break;
      case 5:  // magenta
      default:
         mValues[0] = pBrightness;
         mValues[1] = complement;
         mValues[2] = prev;
         break;
   }
   writeOut();
}

void RgbLed::off() {
   for (uint32_t& v : mValues)
   {
      v = value(0);
   }
   writeOut();
}

uint32_t RgbLed::value(uint32_t pValue) {
   uint32_t out = mCommonAnode ? 256u - pValue : pValue;
   return out;
}

void RgbLed::writeOut() {
   ESP_LOGV(TAG, "write r=%u g=%u b=%u", static_cast<uint32_t>(mValues[0]), static_cast<uint32_t>(mValues[1]), static_cast<uint32_t>(mValues[2]));
   for (size_t i = 0; i < mLeds.size(); i++)
   {
      mLeds[i]->write(mValues[i]);
   }
}

} // namespace Peripherals
} // namespace Bt

