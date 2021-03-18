//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Ui::LedStrip
//
//*************************************************************************************************

#ifndef INC__Bt_Ui_LedStrip__h
#define INC__Bt_Ui_LedStrip__h

#include <vector>

#include "driver/rmt.h"

#include <Bt/Ui/Color.h>

namespace Bt {
namespace Ui {

class LedStrip
{
   public:
      enum Type {SK6812,WS2812B/*,WS2811*/};

      LedStrip(Type pType, gpio_num_t pPin, rmt_channel_t pRtmChannel, size_t pNumberOfLeds);
      LedStrip(const LedStrip&) = delete;
      LedStrip& operator=(const LedStrip&) = delete;
      ~LedStrip();

      void setPixel(size_t pIndex, const Color& pColor);
      void clear();
      void refresh();

   private:
      Type mType;
      size_t mNumberOfLeds;
      std::vector<uint8_t> mBuffer;
      rmt_config_t mConfig;
      
};

} // namespace Ui
} // namespace Bt

#endif // INC__Bt_Ui_LedStrip__h

