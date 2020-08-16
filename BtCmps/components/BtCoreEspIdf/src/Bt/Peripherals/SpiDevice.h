//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Peripherals::SpiDevice
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_SpiDevice__h
#define INC__Bt_Peripherals_SpiDevice__h

#include "Bt/Peripherals/SpiMaster.h"

#include "Bt/Peripherals/I_SpiDevice.h"

namespace Bt {
namespace Peripherals {

class SpiDevice : public I_SpiDevice
{
   public:
      SpiDevice(SpiMaster& pMaster, gpio_num_t pCsPin, int pClockSpeedHz);
      SpiDevice(const SpiDevice&) = delete;
      SpiDevice& operator=(const SpiDevice&) = delete;
      ~SpiDevice();

      virtual void send(uint8_t pData);
      virtual void send(const uint8_t* pData, size_t pLength);

   private:
      SpiMaster& mMaster;
      spi_device_handle_t mHandle;
      spi_device_interface_config_t mConfig;
      
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_SpiDevice__h
