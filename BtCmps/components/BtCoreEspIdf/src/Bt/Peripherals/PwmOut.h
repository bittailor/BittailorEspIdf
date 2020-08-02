//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::PwmOut
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_PwmOut__h
#define INC__Bt_Peripherals_PwmOut__h

#include "Bt/Peripherals/I_PwmOut.h"
#include "Bt/Peripherals/PwmTimer.h"

namespace Bt {
namespace Peripherals {

class PwmOut : public I_PwmOut
{
   public:
      PwmOut(PwmTimer& pTimer, ledc_channel_t pChannel, gpio_num_t pGpio, uint32_t pInitialDuty = 0);
      PwmOut(const PwmOut&) = delete;
      PwmOut& operator=(const PwmOut&) = delete;
      ~PwmOut();

      virtual void write(uint32_t pDuty);

   private:
      ledc_channel_config_t mChannelConfig;
      
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_PwmOut__h
