//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::PwmTimer
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_PwmTimer__h
#define INC__Bt_Peripherals_PwmTimer__h

#include "driver/ledc.h"

#include "Bt/Peripherals/I_PwmTimer.h"

namespace Bt {
namespace Peripherals {

class PwmTimer : public I_PwmTimer
{
   public:
      PwmTimer(ledc_timer_t pTimer, ledc_mode_t pMode, uint32_t pFrequencyInHz, ledc_timer_bit_t pDutyResolution);
      PwmTimer(const PwmTimer&) = delete;
      PwmTimer& operator=(const PwmTimer&) = delete;
      ~PwmTimer();

      virtual void configure(uint32_t pFrequency);

      ledc_timer_t timer() {return mTimerConfig.timer_num;}
      ledc_mode_t mode() {return mTimerConfig.speed_mode;}

   private:
      ledc_timer_config_t mTimerConfig;
      
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_PwmTimer__h
