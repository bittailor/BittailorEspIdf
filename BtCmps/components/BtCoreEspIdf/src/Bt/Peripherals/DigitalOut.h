//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::DigitalOut
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_DigitalOut__h
#define INC__Bt_Peripherals_DigitalOut__h

#include <cstdint>

#include <driver/gpio.h>

#include "Bt/Peripherals/I_DigitalOut.h"

namespace Bt {
namespace Peripherals {

class DigitalOut : public I_DigitalOut
{
   public:
      DigitalOut(gpio_num_t pPin, bool pInitialValue = false);
      DigitalOut(const DigitalOut&) = delete;
      DigitalOut& operator=(const DigitalOut&) = delete;
      ~DigitalOut();

      virtual void write(bool pValue);
      virtual bool value();


   private:
      gpio_num_t mPin;
      bool mValue;
      
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_DigitalOut__h
