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
#include <Bt/Ui/I_LedStrip.h>
#include <Bt/Ui/LedStrip.h>

using namespace std::chrono_literals;

constexpr const char* TAG = "Main";
Bt::Concurrency::CountdownLatch sMainExitLatch(1);
 
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

   Bt::Network::WiFiController wiFiController(mainExecutionContext, defaultEventLoop);
   Bt::Protocols::SntpController sntpController(defaultEventLoop);

   Bt::Protocols::MqttController mqttController(defaultEventLoop);

   Bt::System::OtaUpdate otaUpdate(mainExecutionContext, mqttController);
   Bt::System::Vitals vitals(mainExecutionContext,mqttController);
   
   
   std::vector<std::shared_ptr<Bt::Ui::I_LedStrip>> strips{
      std::make_shared<Bt::Ui::LedStrip>(Bt::Ui::LedStrip::WS2812B, GPIO_NUM_5, RMT_CHANNEL_0, 64),
      std::make_shared<Bt::Ui::LedStrip>(Bt::Ui::LedStrip::SK6812, GPIO_NUM_18, RMT_CHANNEL_1, 45),
      std::make_shared<Bt::Ui::LedStrip>(Bt::Ui::LedStrip::WS2811, GPIO_NUM_23, RMT_CHANNEL_2, 16),
   };
   
   for (auto strip : strips)
   {
      strip->clear();
      strip->refresh();   
   }
   
   uint8_t hsvValue = 35;
   uint8_t hsvSaturation = 255;
   
   std::function<void()> count = [&strips,&hsvSaturation,&hsvValue](){
      for (auto strip : strips)
      {
         for (size_t i = 0; i < strip->numberOfLeds(); i++)
         {
            strip->setPixel(i, Bt::Ui::Color::fromHSV(0, hsvSaturation, hsvValue));
            strip->refresh();
            vTaskDelay(10/portTICK_PERIOD_MS);
         }   
      }  
   };

   std::function<void()> refresh = [&strips,&hsvSaturation,&hsvValue](){
      for (auto strip : strips)
      {
         uint16_t pHueInc = (std::numeric_limits<uint16_t>::max()/strip->numberOfLeds());
            
         for (size_t i = 0; i < strip->numberOfLeds(); i++)
         {
            strip->setPixel(i, Bt::Ui::Color::fromHSV(i*pHueInc, hsvSaturation, hsvValue));
         }
         strip->refresh();
      }
   };

   Bt::Protocols::MqttMessageSubscription mColorValueSubscription(
      mainExecutionContext,
      mqttController, 
      Bt::Core::stringPrintf("bittailor/ledstrip/%s/rgb/hsv/value",Bt::System::getId().c_str()), 
      2, 
      [&refresh,&hsvValue](std::shared_ptr<Bt::Protocols::MqttMessage> pMessage){
         int value = std::stoi(pMessage->data);
         ESP_LOGI(TAG, "mqtt value => %s => %d", pMessage->data.c_str(), value);
         hsvValue = value;
         refresh();
      }
   );
   Bt::Protocols::MqttMessageSubscription mColorSaturationSubscription(
      mainExecutionContext,
      mqttController, 
      Bt::Core::stringPrintf("bittailor/ledstrip/%s/rgb/hsv/saturation",Bt::System::getId().c_str()), 
      2, 
      [&refresh,&hsvSaturation](std::shared_ptr<Bt::Protocols::MqttMessage> pMessage){
         int saturation = std::stoi(pMessage->data);
         ESP_LOGI(TAG, "mqtt saturation => %s => %d", pMessage->data.c_str(), saturation);
         hsvSaturation = saturation;
         refresh();
      }
   );  

   mainExecutionContext.callOnce(10ms,[&count,&refresh,&mainExecutionContext](){
       count();
       mainExecutionContext.callOnce(10ms,[&refresh](){
       refresh();   
   });   
   });

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
