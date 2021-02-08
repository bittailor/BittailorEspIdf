#include <iostream>
#include <queue>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_system.h>

#include <ArduinoJson.h>

#include <Bt/Bluetooth/BleAdvertismentScanner.h>
#include <Bt/Bluetooth/BleController.h>
#include <Bt/Bluetooth/BleDeviceDiscoveryAgent.h>
#include <Bt/Concurrency/CountdownLatch.h>
#include <Bt/Concurrency/SchedulingExecutionContext.h>
#include <Bt/Core/Singleton.h>
#include <Bt/Core/StringBuilder.h>
#include <Bt/Core/Time.h>
#include <Bt/Core/Timezone.h>
#include <Bt/Devices/Xiaomi/BleDiscoveryAgent.h>
#include <Bt/Devices/Xiaomi/DeviceFactory.h>
#include <Bt/Devices/Xiaomi/HumiditySensor.h>
#include <Bt/Events/DefaultEventLoop.h>
#include <Bt/Events/Events.h>
#include <Bt/Network/WiFiController.h>
#include <Bt/Protocols/MqttController.h>
#include <Bt/Protocols/SntpController.h>
#include <Bt/Storage/NvsRepository.h>
#include <Bt/Storage/VirtualFilesystem.h>
#include <Bt/System/OtaUpdate.h>
#include <Bt/System/System.h>
#include <Bt/System/Vitals.h>
#include <Bt/Xiaomi/AdvertisementsGateway.h>



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

   Bt::Network::WiFiController wiFiController(mainExecutionContext, defaultEventLoop, CONFIG_BT_BLE_ADVERTISEMENTS_GATEWAY_WIFI_SSID, CONFIG_BT_BLE_ADVERTISEMENTS_GATEWAY_WIFI_PASSWORD);
   Bt::Protocols::SntpController sntpController(defaultEventLoop);


   Bt::Protocols::MqttController mqttController(defaultEventLoop,
                                                CONFIG_BT_BLE_ADVERTISEMENTS_GATEWAY_MQTT_URI,
                                                [](esp_mqtt_client_config_t& cfg){
                                                      // cfg.client_id = CONFIG_BT_BLE_ADVERTISEMENTS_GATEWAY_MQTT_CLIENT_ID;
                                                      cfg.username = CONFIG_BT_BLE_ADVERTISEMENTS_GATEWAY_MQTT_USERNAME;
                                                      cfg.password = CONFIG_BT_BLE_ADVERTISEMENTS_GATEWAY_MQTT_PASSWORD;
                                                });

   
   Bt::System::OtaUpdate otaUpdate(mainExecutionContext, mqttController);
   Bt::System::Vitals vitals(mainExecutionContext, mqttController);
   Bt::Bluetooth::BleController bleController(mainExecutionContext);
   Bt::Bluetooth::BleAdvertismentScanner bleAdvertismentScanner(mainExecutionContext, bleController);
   Bt::Xiaomi::AdvertisementsGateway advertisementsGateway(mainExecutionContext, bleAdvertismentScanner, mqttController);
   

   ESP_LOGI(TAG, "Run:");
   ESP_LOGI(TAG, " - Free memory : %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, " - Free stack  : %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));

   mainExecutionContext.run();
}

extern "C" void app_main(void) {
   esp_log_level_set("*", ESP_LOG_INFO);
   ESP_LOGI(TAG, "");
   ESP_LOGI(TAG, "*************************************");
   ESP_LOGI(TAG, "**  Bittailor                      **");
   ESP_LOGI(TAG, "**  Mijia BLE Advertisment Gateway **");
   ESP_LOGI(TAG, "*************************************");
   ESP_LOGI(TAG, "");
   ESP_LOGI(TAG, "Main:");
   ESP_LOGI(TAG, " - IDF version   : %s", esp_get_idf_version());
   ESP_LOGI(TAG, " - Free memory   : %d bytes", esp_get_free_heap_size());

   xTaskCreatePinnedToCore(executionContext, "exe-ctx-main", 20480, nullptr, 10, nullptr, 1);

   ESP_LOGI(TAG, "Main exit await: free stack = %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));
   sMainExitLatch.wait();
}
