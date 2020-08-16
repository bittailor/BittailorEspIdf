//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::EPaper
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_EPaper__h
#define INC__Bt_Devices_EPaper__h

#include <Bt/Peripherals/I_DigitalOut.h>
#include <Bt/Peripherals/I_DigitalIn.h>
#include <Bt/Peripherals/I_SpiDevice.h>
#include <Bt/Concurrency/I_SchedulingExecutionContext.h>

namespace Bt {
namespace Devices {

class EPaper
{
   public:
      EPaper(Concurrency::I_SchedulingExecutionContext& pExecutionContext,
             Peripherals::I_SpiDevice& pSpiDevice,
             Peripherals::I_DigitalOut& pDc,
             Peripherals::I_DigitalOut& pReset,
             Peripherals::I_DigitalIn& pBusy);
      EPaper(const EPaper&) = delete;
      EPaper& operator=(const EPaper&) = delete;
      ~EPaper();

      void onInitialized(std::function<void()> pOnInitialized) {mOnInitialized=pOnInitialized;};

      void setFrameMemory(const uint8_t* pImageBuffer);
      void setFrameMemory(const uint8_t* pImageBuffer, int pX, int pY, int pImageWidth, int pImageHeight);
      void clearFrameMemory(bool pWhite = true);
      void displayFrame(std::function<void()> pOnFinished);
      void enablePartial();

   private:
      void init();
      void reset(std::function<void()> pNext);
      void sendLut();
      
      void sendCommand(uint8_t pCmd);
      void sendData(uint8_t pData);
      void sendData(const uint8_t* pData, size_t pLength);
      /*
      template<typename Container>
      void sendData(const Container& pData) {sendData(pData.data().pData.size());}
      */

      void setMemoryArea(int pX1, int pY1, int pX2, int pY2);
      void setMemoryPointer(int pX, int pY);
      void onBackToIdle(std::function<void()> pNext);

      Concurrency::I_SchedulingExecutionContext& mExecutionContext;
      Peripherals::I_SpiDevice& mSpiDevice;
      Peripherals::I_DigitalOut& mDc;
      Peripherals::I_DigitalOut& mReset;
      Peripherals::I_DigitalIn& mBusy;
      size_t mWidth;
      size_t mHeight;
      const uint8_t* mLut;
      uint8_t mUpdateControlData;
      std::function<void()> mOnInitialized;


};

} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_EPaper__h
