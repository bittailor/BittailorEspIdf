//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::SpiDevice
//
//*************************************************************************************************

#include "Bt/Peripherals/SpiDevice.h"

#include "Bt/Peripherals/Tag.h"

#include <cstring>

namespace Bt {
namespace Peripherals {

SpiDevice::SpiDevice(SpiMaster& pMaster, gpio_num_t pCsPin, int pClockSpeedHz)
: mMaster(pMaster), mConfig{} {
   mConfig.clock_speed_hz = pClockSpeedHz; // 10*1000*1000;
   mConfig.mode = 0;
   mConfig.spics_io_num = pCsPin;
   mConfig.input_delay_ns=0;
   mConfig.flags = (SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE);
   mConfig.queue_size = 7;
   //mDeviceConfig.pre_cb=lcd_spi_pre_transfer_callback,
   ESP_ERROR_CHECK(spi_bus_add_device(pMaster.id(), &mConfig, &mHandle));

   gpio_set_direction((gpio_num_t)mConfig.spics_io_num, GPIO_MODE_OUTPUT);
   gpio_set_level((gpio_num_t)mConfig.spics_io_num, 1);

}

SpiDevice::~SpiDevice() {
   ESP_ERROR_CHECK(spi_bus_remove_device(mHandle));
}

void SpiDevice::send(uint8_t pData) {
   //ESP_LOGI(TAG, " *** SpiDevice::send 1 => 0x%x", pData);
   spi_transaction_t t {};
   memset(&t, 0, sizeof(t));
   t.length = 8;
   t.tx_buffer = &pData;
   gpio_set_level((gpio_num_t)mConfig.spics_io_num, 0);
   ESP_ERROR_CHECK(spi_device_polling_transmit(mHandle, &t));
   gpio_set_level((gpio_num_t)mConfig.spics_io_num, 1);
}

void SpiDevice::send(const uint8_t* pData, size_t pLength) {
   //ESP_LOGI(TAG, " *** SpiDevice::send data pLength = %u", pLength);
   if (pLength == 0 || pData == nullptr) {return;}
   spi_transaction_t t {};
   memset(&t, 0, sizeof(t));
   t.length = pLength * 8;
   t.tx_buffer= pData;
   gpio_set_level((gpio_num_t)mConfig.spics_io_num, 0);
   ESP_ERROR_CHECK(spi_device_polling_transmit(mHandle, &t));
   gpio_set_level((gpio_num_t)mConfig.spics_io_num, 1);
}

} // namespace Peripherals
} // namespace Bt
