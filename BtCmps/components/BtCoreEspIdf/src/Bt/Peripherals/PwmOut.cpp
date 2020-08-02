//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::PwmOut
//
//*************************************************************************************************

#include "Bt/Peripherals/PwmOut.h"

#include <mutex>

namespace Bt {
namespace Peripherals {
namespace {
   std::once_flag sOnceFlag;
}

PwmOut::PwmOut(PwmTimer& pTimer, ledc_channel_t pChannel, gpio_num_t pGpio, uint32_t pInitialDuty)
: mChannelConfig{}{
   mChannelConfig.channel    = pChannel;
   mChannelConfig.duty       = 0;
   mChannelConfig.gpio_num   = pGpio;
   mChannelConfig.speed_mode = pTimer.mode();
   mChannelConfig.hpoint     = 0;
   mChannelConfig.timer_sel  = pTimer.timer();

   ESP_ERROR_CHECK(ledc_channel_config(&mChannelConfig));

   std::call_once(sOnceFlag,[]() {
      ESP_ERROR_CHECK(ledc_fade_func_install(0));
   });
}

PwmOut::~PwmOut() {

}

void PwmOut::write(uint32_t pDuty) {
   mChannelConfig.duty = pDuty;
   ESP_ERROR_CHECK(ledc_set_duty_and_update(mChannelConfig.speed_mode,  mChannelConfig.channel, mChannelConfig.duty, mChannelConfig.hpoint));
}

} // namespace Peripherals
} // namespace Bt
