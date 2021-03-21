//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Ui::LedStrip
//
//*************************************************************************************************

#ifndef INC__Bt_Ui_LedStrip__h
#define INC__Bt_Ui_LedStrip__h

#include <vector>

#include <driver/rmt.h>

#include <Bt/Ui/Color.h>
#include <Bt/Ui/I_LedStrip.h>

namespace Bt {
namespace Ui {

class LedStrip : public I_LedStrip
{
   public:
      enum Type {SK6812,WS2812B,WS2811};

      LedStrip(Type pType, gpio_num_t pPin, rmt_channel_t pRtmChannel, size_t pNumberOfLeds);
      LedStrip(const LedStrip&) = delete;
      LedStrip& operator=(const LedStrip&) = delete;
      ~LedStrip();

      virtual void setPixel(size_t pIndex, const Color& pColor);
      virtual void clear();
      virtual void refresh();
      virtual size_t numberOfLeds() const {return mNumberOfLeds;}

   private:
      Type mType;
      size_t mNumberOfLeds;
      std::vector<uint8_t> mBuffer;
      rmt_config_t mConfig;
      
};

} // namespace Ui
} // namespace Bt

#endif // INC__Bt_Ui_LedStrip__h

