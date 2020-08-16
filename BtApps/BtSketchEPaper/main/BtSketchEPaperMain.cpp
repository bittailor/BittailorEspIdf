#include <iostream>
#include <sstream>
#include <inttypes.h>

#include <esp_log.h>
#include <esp_system.h>
#include <soc/rtc.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


#include <Bt/Core/Time.h>
#include <Bt/Core/Timezone.h>
#include <Bt/Concurrency/CountdownLatch.h>
#include <Bt/Concurrency/SchedulingExecutionContext.h>
#include <Bt/Peripherals/DigitalIn.h>
#include <Bt/Peripherals/DigitalOut.h>
#include <Bt/Peripherals/SpiDevice.h>
#include <Bt/Peripherals/SpiMaster.h>
#include <Bt/Storage/NvsRepository.h>
#include <Bt/Devices/EPaper.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>

#include "imagedata.h"
#include "epaper.h"
#include "epaper_fonts.h"
#include "epdpaint.h"

using namespace std::chrono_literals;

// Pin definition
#define MOSI_PIN        23
#define MISO_PIN        19
#define SCK_PIN         18
#define BUSY_PIN        25
#define DC_PIN          27
#define RST_PIN         26
#define CS_PIN          5

// Display resolution296x128
#define EPD_WIDTH       128
#define EPD_HEIGHT      296

// Color inverse. 1 or 0 = set or reset a bit if set a colored pixel
#define IF_INVERT_COLOR 1

constexpr const char* TAG = "BtAlarmClockApp";

Bt::Concurrency::CountdownLatch sMainExitLatch(1);

void displayCounting(Paint& paint, Bt::Devices::EPaper& paper) {
   static int counter = 10;
   std::stringstream msg;
   msg << counter++;
   int offset = (counter % 4 ) * 5;
   ESP_LOGI(TAG, "displayCounting => %s", msg.str().c_str());
   auto t1 = esp_timer_get_time();
   paint.Clear(UNCOLORED);
   auto t2 = esp_timer_get_time();
   paint.DrawRectangle(offset, offset, EPD_HEIGHT-offset, EPD_WIDTH-offset , COLORED);
   paint.DrawRectangle(5 + offset, 5 + offset, EPD_HEIGHT-5-offset, 15 + offset, COLORED);
   paint.DrawStringAt(5 + offset, 15 + offset, "Welcome To Bittailor", &Font16, COLORED);
   paint.DrawStringAt(50, 60, msg.str().c_str(), &Font24, COLORED);
   auto t3 = esp_timer_get_time();
   paper.setFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
   auto t4 = esp_timer_get_time();
   if(counter%20 == 18) {
      paper.enableFull();
   }
   if(counter%20 == 19) {
        paper.enablePartial();
     }
   paper.displayFrame([&paper,&paint,t1,t2,t3,t4](){
      auto t5 = esp_timer_get_time();
      int clear  = t2-t1;
      int draw   = t3-t2;
      int send   = t4-t3;
      int disply = t5-t4;
      ESP_LOGI(TAG, "clear  => %d" , clear);
      ESP_LOGI(TAG, "draw   => %d" , draw);
      ESP_LOGI(TAG, "send   => %d" , send);
      ESP_LOGI(TAG, "disply => %d" , disply);
      displayCounting(paint, paper);
   });

}


void sketchEPaperMain(void* pContext)
{
   ESP_LOGI(TAG, "Startup:");
   ESP_LOGI(TAG, " - Free memory : %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, " - Free stack  : %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));

   Bt::Core::Time time;
   Bt::Core::Timezone timezone;

   Bt::Storage::NvsRepository nvsRepository;
   Bt::Concurrency::SchedulingExecutionContext mainExecutionContext(time);
   Bt::Peripherals::SpiMaster spiMaster(VSPI_HOST, (gpio_num_t)SCK_PIN, (gpio_num_t)MOSI_PIN);
   Bt::Peripherals::SpiDevice ePaperSpiDevice(spiMaster, (gpio_num_t)CS_PIN, 5000000);
   Bt::Peripherals::DigitalOut ePaperDc((gpio_num_t)DC_PIN);
   Bt::Peripherals::DigitalOut ePaperReset((gpio_num_t)RST_PIN);
   Bt::Peripherals::DigitalIn ePaperBusy((gpio_num_t)BUSY_PIN, GPIO_PULLUP_ONLY);

   uint8_t* image = new uint8_t[37888]{0};
   Paint paint(image, 0, 0);    // width should be the multiple of 8
   paint.SetWidth(EPD_WIDTH);
   paint.SetHeight(EPD_HEIGHT);
   paint.SetRotate(ROTATE_90);
   paint.Clear(UNCOLORED);


   Bt::Devices::EPaper paper(mainExecutionContext, ePaperSpiDevice, ePaperDc, ePaperReset, ePaperBusy);

   paper.onInitialized([&paper,&mainExecutionContext,&paint](){
      ESP_LOGI(TAG, "epaper on initialized");
      paper.clearFrameMemory();
      paper.displayFrame([&paper,&mainExecutionContext,&paint](){
         ESP_LOGI(TAG, " clear frame 1 done");
         paper.clearFrameMemory();
         paper.displayFrame([&paper,&mainExecutionContext,&paint](){
            ESP_LOGI(TAG, " clear frame 2 done");
            paint.DrawRectangle(5, 5, EPD_HEIGHT-5, EPD_WIDTH-5 , COLORED);
            paint.DrawRectangle(10, 10, EPD_HEIGHT-10, 20, COLORED);
            paint.DrawStringAt(10, 20, "Welcome To Bittailor 1", &Font16, COLORED);
            paper.setFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
            paper.displayFrame([&paper,&mainExecutionContext,&paint](){
               paper.enablePartial();
               displayCounting(paint,paper);
            });
         });
      });
   });

   mainExecutionContext.run();
}

extern "C" void app_main(void) {
   esp_log_level_set("*", ESP_LOG_DEBUG);

   
   xTaskCreatePinnedToCore(sketchEPaperMain, "exe-ctx-main", 20480, nullptr, 10, nullptr, 1);

   ESP_LOGI(TAG, "Main exit await: free stack = %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));
   sMainExitLatch.wait();
}
