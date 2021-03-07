//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Ui::Color
//
//*************************************************************************************************

#ifndef INC__Bt_Ui_Color__h
#define INC__Bt_Ui_Color__h

#include <cstdint>

namespace Bt {
namespace Ui {

class Color 
{
   public:
   
      constexpr Color()
      : mRed(0), mGreen(0), mBlue(0) {
      }

      constexpr Color(uint8_t iRed, uint8_t iGreen, uint8_t iBlue)
      : mRed(iRed), mGreen(iGreen), mBlue(iBlue) {
      }

      constexpr explicit Color(uint32_t iRgb)
      : mRed(0), mGreen(0), mBlue(0) {
         mRed = (iRgb >> 16) & 0xFF;
         mGreen = (iRgb >> 8) & 0xFF;
         mBlue = iRgb & 0xFF;
      }

      constexpr bool operator==(const Color iColor) const {
         return mRed == iColor.mRed && mGreen == iColor.mGreen && mBlue == iColor.mBlue;
      }

      constexpr uint8_t red() const {
         return mRed;
      }

      constexpr uint8_t green() const {
         return mGreen;
      }

      constexpr uint8_t blue() const {
         return mBlue;
      }

      constexpr Color dimm(uint8_t iFactor) const {
         return Color(mRed/iFactor,mGreen/iFactor,mBlue/iFactor);
      }

      static constexpr Color fromHSV(uint16_t pHue, uint8_t pSat = 255, uint8_t pVal = 255) {
         uint8_t r=0, g=0, b = 0;
         pHue = (pHue * 1530L + 32768) / 65536;
         if(pHue < 510) {
            b = 0;
            if(pHue < 255) {
               r = 255;
               g = pHue;            
            } else {             
               r = 510 - pHue;      
               g = 255;
            }
         } else if(pHue < 1020) { 
            r = 0;
            if(pHue <  765) {      
               g = 255;
               b = pHue - 510;
            } else {             
               g = 1020 - pHue;    
               b = 255;
            }
         } else if(pHue < 1530) { 
            g = 0;
            if(pHue < 1275) {     
               r = pHue - 1020; 
               b = 255;
            } else {              
               r = 255;
               b = 1530 - pHue;     
            }
         } else {                
            r = 255;
            g = b = 0;
         }

         uint32_t v1 =   1 + pVal; 
         uint16_t s1 =   1 + pSat;
         uint8_t  s2 = 255 - pSat;
         return Color(
            ((((r * s1) >> 8) + s2) * v1) >> 8,
            ((((g * s1) >> 8) + s2) * v1) >> 8,
            ((((b * s1) >> 8) + s2) * v1) >> 8
         );         
      }

   private:
      uint8_t mRed;
      uint8_t mGreen;
      uint8_t mBlue;

};



} // namespace Ui
} // namespace Bt

#endif // INC__Bt_Ui_Color__h

