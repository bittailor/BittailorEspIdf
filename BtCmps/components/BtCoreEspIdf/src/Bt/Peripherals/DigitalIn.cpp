//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::DigitalIn
//
//*************************************************************************************************

#include "Bt/Peripherals/DigitalIn.h"

namespace Bt {
namespace Peripherals {

DigitalIn::DigitalIn(gpio_num_t pPin, gpio_pull_mode_t pPullMode) : mPin(pPin) {
   gpio_pad_select_gpio(mPin);
   ESP_ERROR_CHECK(gpio_set_direction(mPin, GPIO_MODE_INPUT));
   ESP_ERROR_CHECK(gpio_set_pull_mode(mPin, pPullMode));


}

DigitalIn::~DigitalIn() {

}

bool DigitalIn::read() {
   return gpio_get_level(mPin) > 0;
}

} // namespace Peripherals
} // namespace Bt
