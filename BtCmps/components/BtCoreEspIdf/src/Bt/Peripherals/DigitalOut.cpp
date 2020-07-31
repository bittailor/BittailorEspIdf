//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::DigitalOut
//
//*************************************************************************************************

#include "Bt/Peripherals/DigitalOut.h"

namespace Bt {
namespace Peripherals {

DigitalOut::DigitalOut(gpio_num_t pPin, bool pInitialValue) : mPin(pPin) {
   gpio_pad_select_gpio(mPin);
   ESP_ERROR_CHECK(gpio_set_direction(mPin, GPIO_MODE_OUTPUT));
   write(pInitialValue);
}

DigitalOut::~DigitalOut() {

}

void DigitalOut::write(bool pValue) {
   mValue = pValue;
   gpio_set_level(mPin, mValue ? 1 : 0);
}

bool DigitalOut::value() {
   return mValue;
}

} // namespace Peripherals
} // namespace Bt
