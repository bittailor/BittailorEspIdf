//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::SpiMaster
//
//*************************************************************************************************

#include "Bt/Peripherals/SpiMaster.h"

#include "Bt/Peripherals/Tag.h"

namespace Bt {
namespace Peripherals {

SpiMaster::SpiMaster(spi_host_device_t pSpiId, gpio_num_t pSclkPin, gpio_num_t pMosiPin, gpio_num_t pMisoPin, int pDmaChannel)
: mSpiId(pSpiId), mBusConfig{} {
   mBusConfig.mosi_io_num = pMosiPin;
   mBusConfig.miso_io_num = pMisoPin;
   mBusConfig.sclk_io_num = pSclkPin;
   mBusConfig.quadwp_io_num=-1;
   mBusConfig.quadhd_io_num=-1;
   mBusConfig.max_transfer_sz=4094;
   mBusConfig.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI ;

   ESP_LOGI(TAG, " *** mBusConfig.flags %d", mBusConfig.flags);
   ESP_LOGI(TAG, " *** mBusConfig.intr_flags %d", mBusConfig.intr_flags);


   ESP_ERROR_CHECK(spi_bus_initialize(mSpiId, &mBusConfig, pDmaChannel));

}

SpiMaster::~SpiMaster() {
   ESP_ERROR_CHECK(spi_bus_free(mSpiId));
}

} // namespace Peripherals
} // namespace Bt
