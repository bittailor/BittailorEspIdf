//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::DisplayBacklight
//
//*************************************************************************************************

#include "Bt/AlarmClock/DisplayBacklight.h"

#include "Bt/AlarmClock/Tag.h"

namespace Bt {
namespace AlarmClock {



DisplayBacklight::DisplayBacklight(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Peripherals::I_RgbLed& pRgbLed)
: mExecutionContext(pExecutionContext), mRgbLed(pRgbLed), mHue(0), mBrightness(100), mAutoOffTime(10000) ,mState(false) {
}

DisplayBacklight::~DisplayBacklight() {
}

void DisplayBacklight::configure(uint8_t pHue, uint8_t pBrightness, const std::chrono::milliseconds& pAutoOffTime) {
   ESP_LOGI(TAG, "hue = %u brightness = %u autoOffTime=%u", static_cast<uint32_t>(pHue), static_cast<uint32_t>(pBrightness), static_cast<uint32_t>(pAutoOffTime.count())); 
   mHue = pHue;
   mBrightness = pBrightness;
   mAutoOffTime = pAutoOffTime;
   if(mState) {
      mRgbLed.setHue(mHue, mBrightness);
   }
}

void DisplayBacklight::state(bool pOn) {
   mState = pOn;
   if (pOn) {
      ESP_LOGI(TAG, "on hue = %u brightness = %u autoOffTime=%u", static_cast<uint32_t>(mHue), static_cast<uint32_t>(mBrightness), static_cast<uint32_t>(mAutoOffTime.count())); 
      mRgbLed.setHue(mHue, mBrightness);
      mAutoOffTimer = mExecutionContext.callOnce(mAutoOffTime, [this]{
         mState = false;
         mRgbLed.off();
      });
   } else {
      if(mAutoOffTimer) {
         mAutoOffTimer->cancel();
         mAutoOffTimer.reset();
      }
      mRgbLed.off();
   }
}

} // namespace AlarmClock
} // namespace Bt

