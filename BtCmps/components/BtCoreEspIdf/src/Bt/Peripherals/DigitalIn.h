//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::DigitalIn
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_DigitalIn__h
#define INC__Bt_Peripherals_DigitalIn__h

#include <driver/gpio.h>

#include "Bt/Peripherals/I_DigitalIn.h"

namespace Bt {
namespace Peripherals {

class DigitalIn : public I_DigitalIn
{
   public:
      DigitalIn(gpio_num_t pPin, gpio_pull_mode_t pPullMode);
      DigitalIn(const DigitalIn&) = delete;
      DigitalIn& operator=(const DigitalIn&) = delete;
      ~DigitalIn();

      virtual bool read();

   private:
      gpio_num_t mPin;
      
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_DigitalIn__h
