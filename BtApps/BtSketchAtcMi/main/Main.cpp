#include <iostream>
#include <queue>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_system.h>

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

using namespace std::chrono_literals;

constexpr Bt::Bluetooth::BleUuid cServiceUuid =  
            Bt::Bluetooth::BleUuid::from128BitBE(
               Bt::Bluetooth::BleUuid::Uuid128Bit{
                0x00,0x00,0x18,0x1a, 0x00,0x00, 0x10, 0x00, 0x80,0x00, 0x00,0x80,0x5f,0x9b,0x34,0xfb
            });

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
   Bt::Storage::VirtualFilesystem virtualFilesystem;

   virtualFilesystem.registerSpiffsPartition("/default", "default");
   virtualFilesystem.registerSpiffsPartition("/config", "config", true);

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

   Bt::Bluetooth::BleController bleController(mainExecutionContext);
   std::shared_ptr<Bt::Bluetooth::I_BleDeviceDiscoveryAgent> discoveryAgent;
   
   
   Bt::Events::Subscription<Bt::Bluetooth::I_BleController::Synced> onBleSynced(mainExecutionContext, [&mqttController,&bleController,&discoveryAgent,&mainExecutionContext](auto pEvent){
      discoveryAgent = bleController.createDeviceDiscoveryAgent(
         [&mqttController](std::shared_ptr<Bt::Bluetooth::BleDeviceInfo> pDeviceInfo){
            if(pDeviceInfo->serviceUuid() == cServiceUuid) {
               if(pDeviceInfo->serviceData().size() == 15) {
                  float temperature = ((pDeviceInfo->serviceData()[7]<<8)|(pDeviceInfo->serviceData()[6])) * 0.01;
			         float humidity = ((pDeviceInfo->serviceData()[9]<<8)|(pDeviceInfo->serviceData()[8])) * 0.01;
                  float vbattery = ((pDeviceInfo->serviceData()[11]<<8)|(pDeviceInfo->serviceData()[10])) * 0.001;
			         uint8_t battery = pDeviceInfo->serviceData()[12];
			         uint8_t cnt = pDeviceInfo->serviceData()[13];
			         uint8_t flg = pDeviceInfo->serviceData()[14];
                  
                  std::string msg = Bt::Core::stringPrintf("[%s] [%03d] temperature = %.1f : humidity = %.1f : vbattery = %.1f : battery = %d, cnt = %d, flg = %d", 
                     pDeviceInfo->address().toString().c_str(), (int32_t)pDeviceInfo->rssi(), 
                     temperature, humidity, vbattery,
                     battery, (uint32_t)cnt, (uint32_t)flg);
                  
                  ESP_LOGI(TAG, "%s", msg.c_str());

                  // ESP_LOGI(TAG, "[%s] [%03d] temperature = %.1f : humidity = %.1f : vbattery = %.1f : battery = %d, cnt = %d, flg = %d", 
                  //    pDeviceInfo->address().toString().c_str(), (int32_t)pDeviceInfo->rssi(), 
                  //    temperature, humidity, vbattery,
                  //    battery, (uint32_t)cnt, (uint32_t)flg);

                  mqttController.publish(
                     Bt::Core::stringPrintf("bittailor/develop/tests/adv/%s/reading", pDeviceInfo->address().toString().c_str()).c_str(),
                     msg
                  );
               } else {
                  ESP_LOGW(TAG, "Wrong length %d != 15 for %s", pDeviceInfo->serviceData().size(), pDeviceInfo->serviceUuid().toString().c_str());   
               }   
            }
         },
         [&mainExecutionContext,&discoveryAgent](const std::vector<std::shared_ptr<Bt::Bluetooth::BleDeviceInfo>>&){
            ESP_LOGI(TAG, "End Of Scan");
            mainExecutionContext.callOnce(10ms,[&discoveryAgent](){
               ESP_LOGI(TAG, "resstart discovery");
               discoveryAgent->start(1s);
            });   
         }
      );
      ESP_LOGI(TAG, "Start discovery");
      discoveryAgent->start(std::chrono::milliseconds::max());
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
