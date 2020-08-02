//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::PwmTimer
//
//*************************************************************************************************

#include "Bt/Peripherals/PwmTimer.h"

#include <Bt/Peripherals/Tag.h>

namespace Bt {
namespace Peripherals {

PwmTimer::PwmTimer(ledc_timer_t pTimer, ledc_mode_t pMode, uint32_t pFrequencyInHz, ledc_timer_bit_t pDutyResolution)
: mTimerConfig{} {
   mTimerConfig.timer_num = pTimer;
   mTimerConfig.speed_mode = pMode;
   mTimerConfig.freq_hz = pFrequencyInHz;
   mTimerConfig.duty_resolution = pDutyResolution;
   mTimerConfig.clk_cfg = LEDC_AUTO_CLK;

   ESP_ERROR_CHECK(ledc_timer_config(&mTimerConfig));
}

PwmTimer::~PwmTimer() {

}

void PwmTimer::configure(uint32_t pFrequency) {
   esp_err_t result = ledc_set_freq(mTimerConfig.speed_mode, mTimerConfig.timer_num, pFrequency);
   if(result != ESP_OK) {
      ESP_ERROR_CHECK_WITHOUT_ABORT(result);
      return;
   }
   mTimerConfig.freq_hz = pFrequency;
}

} // namespace Peripherals
} // namespace Bt
