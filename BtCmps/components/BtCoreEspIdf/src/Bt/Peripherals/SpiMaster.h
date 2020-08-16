//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::SpiMaster
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_SpiMaster__h
#define INC__Bt_Peripherals_SpiMaster__h

#include <driver/gpio.h>
#include <driver/spi_master.h>

namespace Bt {
namespace Peripherals {

class SpiMaster
{
   public:
      SpiMaster(spi_host_device_t pSpiId, gpio_num_t pSclkPin, gpio_num_t pMosiPin, gpio_num_t pMisoPin = static_cast<gpio_num_t>(-1), int pDmaChannel = 2);
      SpiMaster(const SpiMaster&) = delete;
      SpiMaster& operator=(const SpiMaster&) = delete;
      ~SpiMaster();

      spi_host_device_t id() const {return mSpiId;}

   private:
      spi_host_device_t mSpiId;
      spi_bus_config_t mBusConfig;
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_SpiMaster__h
