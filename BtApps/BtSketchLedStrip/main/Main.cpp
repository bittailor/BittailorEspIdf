#include <iostream>
#include <queue>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_system.h>
#include <esp_task_wdt.h>

#include <Bt/Concurrency/CountdownLatch.h>
#include <Bt/Concurrency/WorkerExecutionContext.h>
#include <Bt/Concurrency/SchedulingExecutionContext.h>
#include <Bt/Core/StringBuilder.h>
#include <Bt/Core/StringUtilities.h>
#include <Bt/Core/Singleton.h>
#include <Bt/Bluetooth/BleController.h>
#include <Bt/Core/Time.h>
#include <Bt/Core/Timezone.h>
#include <Bt/Events/Events.h>
#include <Bt/Events/DefaultEventLoop.h>
#include <Bt/Network/WiFiController.h>
#include <Bt/Protocols/MqttController.h>
#include <Bt/Protocols/SntpController.h>
#include <Bt/Storage/NvsRepository.h>
#include <Bt/Storage/VirtualFilesystem.h>
#include <Bt/System/System.h>
#include <Bt/System/OtaUpdate.h>
#include <Bt/System/Vitals.h>
#include <Bt/Ui/LedStrip.h>

using namespace std::chrono_literals;

constexpr Bt::Bluetooth::BleUuid cServiceUuid =  
            Bt::Bluetooth::BleUuid::from128BitBE(
               Bt::Bluetooth::BleUuid::Uuid128Bit{
                0x00,0x00,0x18,0x1a, 0x00,0x00, 0x10, 0x00, 0x80,0x00, 0x00,0x80,0x5f,0x9b,0x34,0xfb
            });

constexpr const char* TAG = "Main";
Bt::Concurrency::CountdownLatch sMainExitLatch(1);

constexpr size_t NUMBER_OF_LEDS = 45;
 
void executionContext(void* pContext)
{
   ESP_LOGI(TAG, "Startup:");
   ESP_LOGI(TAG, " - Free memory : %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, " - Free stack  : %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));


   Bt::System::System system;
   Bt::Core::Singleton<Bt::System::I_System>::Instance systemSingleton(system); 
   
   Bt::Core::Time time;
   Bt::Core::Timezone timezone;

   Bt::Storage::NvsRepository nvsRepository;
   
   Bt::Events::DefaultEventLoop defaultEventLoop;

   Bt::Concurrency::SchedulingExecutionContext mainExecutionContext(time);

   Bt::Network::WiFiController wiFiController(mainExecutionContext, defaultEventLoop, CONFIG_BT_SKETCH_ATC_MI_WIFI_SSID, CONFIG_BT_SKETCH_ATC_MI_WIFI_PASSWORD);
   Bt::Protocols::SntpController sntpController(defaultEventLoop);


   Bt::Protocols::MqttController mqttController(defaultEventLoop,
                                                "mqtt://piOne.local",
                                                [](esp_mqtt_client_config_t& cfg){
                                                      cfg.username = "***REMOVED***";
                                                      cfg.password = "***REMOVED***";

                                                });

   Bt::System::OtaUpdate otaUpdate(mainExecutionContext, mqttController);
   Bt::System::Vitals vitals(mainExecutionContext,mqttController);
   
   Bt::Ui::LedStrip mLedStrip(GPIO_NUM_5, RMT_CHANNEL_0, NUMBER_OF_LEDS);
   mLedStrip.clear();
   mLedStrip.refresh();
   
   size_t index = 0;
   uint16_t pHue = 0;
   uint8_t pVal = 50;
   //uint16_t pHueInc = (std::numeric_limits<uint16_t>::max()/NUMBER_OF_LEDS)/2;
   uint16_t pHueInc = (std::numeric_limits<uint16_t>::max()/NUMBER_OF_LEDS);
   mLedStrip.clear();
   mLedStrip.setPixel(index++, Bt::Ui::Color::fromHSV(pHue,255,10));
   mLedStrip.refresh();

   mainExecutionContext.callPeriodically(10ms,[&mLedStrip,&index,&pHue,&pHueInc,&pVal](auto& pTimer){
      pHue+=pHueInc;
      //mLedStrip.clear();
      mLedStrip.setPixel(index++, Bt::Ui::Color::fromHSV(pHue, 255, pVal));
      mLedStrip.refresh();
      index = index % NUMBER_OF_LEDS;
      
      /*
      if(index == 0) {
         pVal+=2;   
      }
      */

      esp_task_wdt_reset();
   });

   Bt::Protocols::MqttMessageSubscription mColorValueSubscription(
      mainExecutionContext,
      mqttController, 
      Bt::Core::stringPrintf("bittailor/ledstrip/%s/rgb/hsv/value",Bt::System::getId().c_str()), 
      2, 
      [&pVal](std::shared_ptr<Bt::Protocols::MqttMessage> pMessage){
         int value = std::stoi(pMessage->data);
         ESP_LOGI(TAG, "mqtt => %s => %d", pMessage->data.c_str(), value);
         pVal = value;
      }
   ); 

   mainExecutionContext.callPeriodically(10s,[](auto& pTimer){
       ESP_LOGI(TAG, "10s tick");   
   });

   ESP_LOGI(TAG, "Run:");
   ESP_LOGI(TAG, " - Free memory : %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, " - Free stack  : %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));

   mainExecutionContext.run();
}





extern "C" void app_main(void) {
   esp_log_level_set("*", ESP_LOG_INFO);

   ESP_LOGI(TAG, "Main:");
   ESP_LOGI(TAG, " - IDF version   : %s", esp_get_idf_version());
   ESP_LOGI(TAG, " - Free memory   : %d bytes", esp_get_free_heap_size());

   xTaskCreatePinnedToCore(executionContext, "exe-ctx-main", 20480, nullptr, 10, nullptr, 1);

   ESP_LOGI(TAG, "Main exit await: free stack = %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));
   sMainExitLatch.wait();
}