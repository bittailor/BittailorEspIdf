#include <iostream>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_system.h>

#include <Bt/Bluetooth/BleController.h>
#include <Bt/Bluetooth/BleDeviceDiscoveryAgent.h>
#include <Bt/Concurrency/CountdownLatch.h>
#include <Bt/Concurrency/SchedulingExecutionContext.h>
#include <Bt/Core/StringBuilder.h>
#include <Bt/Core/Time.h>
#include <Bt/Core/Timezone.h>
#include <Bt/Events/Events.h>
#include <Bt/Events/DefaultEventLoop.h>
#include <Bt/Network/WiFiController.h>
#include <Bt/Protocols/MqttController.h>
#include <Bt/Protocols/SntpController.h>
#include <Bt/Storage/NvsRepository.h>
#include <Bt/Storage/VirtualFilesystem.h>

constexpr const char* TAG = "Main";
Bt::Concurrency::CountdownLatch sMainExitLatch(1);

void executionContext(void* pContext)
{
   ESP_LOGI(TAG, "Startup:");
   ESP_LOGI(TAG, " - Free memory : %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, " - Free stack  : %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));

   Bt::Core::Time time;
   Bt::Core::Timezone timezone;

   Bt::Storage::NvsRepository nvsRepository;
   Bt::Storage::VirtualFilesystem virtualFilesystem;

   virtualFilesystem.registerSpiffsPartition("/default", "default");
   virtualFilesystem.registerSpiffsPartition("/config", "config", true);

   Bt::Events::DefaultEventLoop defaultEventLoop;

   Bt::Concurrency::SchedulingExecutionContext mainExecutionContext(time);

   Bt::Network::WiFiController wiFiController(mainExecutionContext, defaultEventLoop, CONFIG_BT_MIIJA_GATEWAY_WIFI_SSID, CONFIG_BT_MIIJA_GATEWAY_WIFI_PASSWORD);
   Bt::Protocols::SntpController sntpController(defaultEventLoop);


   Bt::Protocols::MqttController mqttController(defaultEventLoop,
                                                CONFIG_BT_MIIJA_GATEWAY_MQTT_URI,
                                                [](esp_mqtt_client_config_t& cfg){
                                                      cfg.client_id = CONFIG_BT_MIIJA_GATEWAY_MQTT_CLIENT_ID;
                                                      cfg.username = CONFIG_BT_MIIJA_GATEWAY_MQTT_USERNAME;
                                                      cfg.password = CONFIG_BT_MIIJA_GATEWAY_MQTT_PASSWORD;
                                                });

   int status = 0;
   std::function<void()> startScan = [&status, &mainExecutionContext, &mqttController](){
      status++;
      ESP_LOGI(TAG, "startScan status = %d", status);
      if(status < 2) {
         return;
      }
      ESP_LOGI(TAG, "start discover");
      auto discoveryAgent = new Bt::Bluetooth::BleDeviceDiscoveryAgent([&mainExecutionContext, &mqttController](auto pDeviceInfo){
         if(!pDeviceInfo->serviceUuid().isEmpty() && pDeviceInfo->serviceUuid() == 0xfe95 ) {
            ESP_LOGI(TAG, "DeviceInfo with serviceUuid %s address %s",
               pDeviceInfo->serviceUuid().toString().c_str(),
               pDeviceInfo->address().toString().c_str());   
            mainExecutionContext.call([&mqttController, pDeviceInfo](){
               mqttController.publish("home/sensor/mijia/discover", pDeviceInfo->address().toString());    
            }); 
         }
      });
      discoveryAgent->interval(std::chrono::milliseconds(35));
      discoveryAgent->window(std::chrono::milliseconds(20));
      discoveryAgent->duplicateFilter(true);

      discoveryAgent->start(std::chrono::seconds(600));
   };


   Bt::Events::Subscription<Bt::Bluetooth::I_BleController::Synced> onBleSynced(mainExecutionContext,[&startScan](auto pEvent){
      ESP_LOGI(TAG, "onBleSynced");
      startScan();
   });
   Bt::Events::Subscription<Bt::Protocols::I_MqttController::Connected> onMqttConnected(mainExecutionContext,[&startScan](auto pEvent){
      ESP_LOGI(TAG, "onMqttConnected");
      startScan();
   });


   Bt::Bluetooth::BleController bleController;


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
