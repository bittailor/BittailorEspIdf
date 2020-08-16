//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::EPaper
//
//*************************************************************************************************

#include "Bt/Devices/EPaper.h"

#include "Bt/Devices/Tag.h"

using namespace std::chrono_literals;

namespace Bt {
namespace Devices {
namespace {

// Display resolution
#define EPD_WIDTH       128
#define EPD_HEIGHT      296

// EPD2IN9 commands
constexpr uint8_t DRIVER_OUTPUT_CONTROL                  =0x01;
constexpr uint8_t BOOSTER_SOFT_START_CONTROL             =0x0C;
constexpr uint8_t GATE_SCAN_START_POSITION               =0x0F;
constexpr uint8_t DEEP_SLEEP_MODE                        =0x10;
constexpr uint8_t DATA_ENTRY_MODE_SETTING                =0x11;
constexpr uint8_t SW_RESET                               =0x12;
constexpr uint8_t TEMPERATURE_SENSOR_CONTROL             =0x1A;
constexpr uint8_t MASTER_ACTIVATION                      =0x20;
constexpr uint8_t DISPLAY_UPDATE_CONTROL_1               =0x21;
constexpr uint8_t DISPLAY_UPDATE_CONTROL_2               =0x22;
constexpr uint8_t WRITE_RAM                              =0x24;
constexpr uint8_t WRITE_VCOM_REGISTER                    =0x2C;
constexpr uint8_t WRITE_LUT_REGISTER                     =0x32;
constexpr uint8_t SET_DUMMY_LINE_PERIOD                  =0x3A;
constexpr uint8_t SET_GATE_TIME                          =0x3B;
constexpr uint8_t BORDER_WAVEFORM_CONTROL                =0x3C;
constexpr uint8_t SET_RAM_X_ADDRESS_START_END_POSITION   =0x44;
constexpr uint8_t SET_RAM_Y_ADDRESS_START_END_POSITION   =0x45;
constexpr uint8_t SET_RAM_X_ADDRESS_COUNTER              =0x4E;
constexpr uint8_t SET_RAM_Y_ADDRESS_COUNTER              =0x4F;
constexpr uint8_t TERMINATE_FRAME_READ_WRITE             =0xFF;

template <typename... T>
constexpr auto array_of(T&&... t) {
    return std::array{ static_cast<std::common_type_t<T...>>(t)... };
}


static const uint8_t sLutFullUpdate[] = {
    0x50, 0xAA, 0x55, 0xAA, 0x11, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0x1F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t sLutPartialUpdate[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18,
    0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

} // namespace

EPaper::EPaper(Concurrency::I_SchedulingExecutionContext& pExecutionContext,
               Peripherals::I_SpiDevice& pSpiDevice,
               Peripherals::I_DigitalOut& pDc,
               Peripherals::I_DigitalOut& pReset,
               Peripherals::I_DigitalIn& pBusy)
: mExecutionContext(pExecutionContext)
, mSpiDevice(pSpiDevice)
, mDc(pDc)
, mReset(pReset)
, mBusy(pBusy)
, mWidth(EPD_WIDTH)
, mHeight(EPD_HEIGHT)
, mLut(sLutFullUpdate){
   mExecutionContext.call([this](){init();});
}

EPaper::~EPaper() {

}

void EPaper::init() {
   ESP_LOGI(TAG, "EPaper::init() ...");
   ESP_LOGI(TAG, " Busy before init %d", mBusy.read());
   reset([this](){
      ESP_LOGI(TAG, " Busy after reset %d", mBusy.read());
      sendData((EPD_HEIGHT - 1) & 0xFF);
      sendData(((EPD_HEIGHT - 1) >> 8) & 0xFF);
      sendData(0x00);
      sendCommand(BOOSTER_SOFT_START_CONTROL);
      sendData(0xD7);
      sendData(0xD6);
      sendData(0x9D);
      sendCommand(WRITE_VCOM_REGISTER);
      sendData(0xA8);
      sendCommand(SET_DUMMY_LINE_PERIOD);
      sendData(0x1A);
      sendCommand(SET_GATE_TIME);
      sendData(0x08);
      sendCommand(DATA_ENTRY_MODE_SETTING);
      sendData(0x03);
      sendLut();
      ESP_LOGI(TAG, " Busy after lut %d", mBusy.read());
      ESP_LOGI(TAG, "EPaper::init done");
      if(mOnInitialized) {
         mOnInitialized();
      }
   });
}

void EPaper::sendLut() {
   sendCommand(WRITE_LUT_REGISTER);
   sendData(mLut, std::size(sLutFullUpdate));
}

void EPaper::reset(std::function<void()> pNext) {
   ESP_LOGI(TAG, " Busy before mReset %d", mBusy.read());
   mReset.write(false);
   mExecutionContext.callOnce(200ms, [this,pNext](){
      ESP_LOGI(TAG, " Busy on mReset %d", mBusy.read());
      mReset.write(true);
      mExecutionContext.callOnce(200ms,pNext);
   });
}

void EPaper::sendCommand(uint8_t pCmd) {
   mDc.write(false);
   mSpiDevice.send(pCmd);
}

void EPaper::sendData(uint8_t pData) {
   mDc.write(true);
   mSpiDevice.send(pData);
}

void EPaper::sendData(const uint8_t* pData, size_t pLength) {
   mDc.write(true);
   mSpiDevice.send(pData, pLength);
   /*
   for(uint8_t data : pData) {
      mSpiDevice.send(data);
   }
   */
}

void EPaper::setFrameMemory(const uint8_t* pImageBuffer) {
   setMemoryArea(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
   setMemoryPointer(0, 0);
   sendCommand(WRITE_RAM);
   sendData(pImageBuffer, mWidth / 8 * mHeight);
}

void EPaper::setFrameMemory(const uint8_t* pImageBuffer, int pX, int pY, int pImageWidth, int pImageHeight) {
    int xEnd;
    int yEnd;

    if (pImageBuffer == NULL ||
        pX < 0 || pImageWidth < 0 ||
        pY < 0 || pImageHeight < 0) {
        return;
    }
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    pX &= 0xF8;
    pImageWidth &= 0xF8;
    if (pX + pImageWidth >= mWidth) {
        xEnd = mWidth - 1;
    } else {
        xEnd = pX + pImageWidth - 1;
    }
    if (pY + pImageHeight >= mHeight) {
        yEnd = mHeight - 1;
    } else {
        yEnd = pY + pImageHeight - 1;
    }
    setMemoryArea(pX, pY, xEnd, yEnd);
    setMemoryPointer(pX, pY);
    sendCommand(WRITE_RAM);

    for (int j = 0; j < yEnd - pY + 1; j++) {
        for (int i = 0; i < (xEnd - pX + 1) / 8; i++) {
            sendData(pImageBuffer[i + j * (pImageWidth / 8)]);
        }
    }
}

void EPaper::clearFrameMemory(bool pWhite) {
   setMemoryArea(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
   setMemoryPointer(0, 0);
   sendCommand(WRITE_RAM);
   for (int i = 0; i < EPD_WIDTH / 8 * EPD_HEIGHT; i++) {
     sendData(pWhite ? 0xFF : 0x00);
   }
}

void EPaper::displayFrame(std::function<void()> pOnFinished) {
   //ESP_LOGI(TAG, "EPaper::displayFrame() ...");
   //ESP_LOGI(TAG, " Busy displayFrame 1 %d", mBusy.read());
   sendCommand(DISPLAY_UPDATE_CONTROL_2);
   sendData(0xC4);
   //SendData(0xCC);
   //ESP_LOGI(TAG, " Busy displayFrame 2 %d", mBusy.read());
   sendCommand(MASTER_ACTIVATION);
   //ESP_LOGI(TAG, " Busy displayFrame 3 %d", mBusy.read());
   sendCommand(TERMINATE_FRAME_READ_WRITE);
   //ESP_LOGI(TAG, " Busy displayFrame 4 %d", mBusy.read());
   //ESP_LOGI(TAG, "EPaper::displayFrame() ...");
   onBackToIdle(pOnFinished);
}

void EPaper::enablePartial() {
   mLut = sLutPartialUpdate;
   sendLut();
}


void EPaper::setMemoryArea(int pX1, int pY1, int pX2, int pY2) {
    sendCommand(SET_RAM_X_ADDRESS_START_END_POSITION);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    sendData((pX1 >> 3) & 0xFF);
    sendData((pX2 >> 3) & 0xFF);

    sendCommand(SET_RAM_Y_ADDRESS_START_END_POSITION);
    sendData(pY1 & 0xFF);
    sendData((pY1 >> 8) & 0xFF);
    sendData(pY2 & 0xFF);
    sendData((pY2 >> 8) & 0xFF);
}

void EPaper::setMemoryPointer(int pX, int pY) {
    sendCommand(SET_RAM_X_ADDRESS_COUNTER);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    sendData((pX >> 3) & 0xFF);
    sendCommand(SET_RAM_Y_ADDRESS_COUNTER);
    sendData(pY & 0xFF);
    sendData((pY >> 8) & 0xFF);
}

void EPaper::onBackToIdle(std::function<void()> pNext) {
   mExecutionContext.callPeriodically(100ms, [this,pNext](auto& pTimer){
      ESP_LOGV(TAG, " Busy onBackToIdle %d", mBusy.read());
      if(mBusy.read()){
         ESP_LOGV(TAG, "EPaper::onBackToIdle => busy");
         return;
      }
      ESP_LOGV(TAG, "EPaper::onBackToIdle => back to idle");
      pTimer.cancel();
      if(pNext) {
         mExecutionContext.call(pNext);
      }
   });
}


} // namespace Devices
} // namespace Bt
