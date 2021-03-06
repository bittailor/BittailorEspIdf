#include <iostream>
#include <queue>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_system.h>

#include <ArduinoJson.h>

#include <Bt/Bluetooth/BleController.h>
#include <Bt/Bluetooth/BleDeviceDiscoveryAgent.h>
#include <Bt/Concurrency/CountdownLatch.h>
#include <Bt/Concurrency/SchedulingExecutionContext.h>
#include <Bt/Core/StringBuilder.h>
#include <Bt/Core/Singleton.h>
#include <Bt/Core/Time.h>
#include <Bt/Core/Timezone.h>
#include <Bt/Devices/Xiaomi/DeviceFactory.h>
#include <Bt/Devices/Xiaomi/HumiditySensor.h>
#include <Bt/Devices/Xiaomi/BleDiscoveryAgent.h>
#include <Bt/Events/Events.h>
#include <Bt/Events/DefaultEventLoop.h>
#include <Bt/Network/WiFiController.h>
#include <Bt/Protocols/MqttController.h>
#include <Bt/Protocols/SntpController.h>
#include <Bt/Storage/NvsRepository.h>
#include <Bt/Storage/VirtualFilesystem.h>
#include <Bt/System/System.h>
#include <Bt/System/OtaUpdate.h>
#include <Bt/Xiaomi/Gateway.h>
#include <Bt/System/Vitals.h>



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

   Bt::Network::WiFiController wiFiController(mainExecutionContext, defaultEventLoop);
   Bt::Protocols::SntpController sntpController(defaultEventLoop);


   Bt::Protocols::MqttController mqttController(defaultEventLoop);
   
   Bt::System::OtaUpdate otaUpdate(mainExecutionContext, mqttController);
   Bt::System::Vitals vitals(mainExecutionContext,mqttController);
   Bt::Bluetooth::BleController bleController(mainExecutionContext);
   
   Bt::Devices::Xiaomi::DeviceFactory mXiaomiDeviceFactory;
   Bt::Devices::Xiaomi::HumiditySensor::registerAtFactory(mainExecutionContext, mXiaomiDeviceFactory, bleController) ;
   Bt::Devices::Xiaomi::BleDiscoveryAgent xiaomiBleDiscoveryAgent(bleController);

   Bt::Xiaomi::Gateway xiaomiGateway(mainExecutionContext, bleController, mqttController);   


   ESP_LOGI(TAG, "Run:");
   ESP_LOGI(TAG, " - Free memory : %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, " - Free stack  : %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));

   mainExecutionContext.run();
}

extern "C" void app_main(void) {
   esp_log_level_set("*", ESP_LOG_INFO);
   ESP_LOGI(TAG, "");
   ESP_LOGI(TAG, "*******************************");
   ESP_LOGI(TAG, "**  Bittailor                **");
   ESP_LOGI(TAG, "**  Mijia BLE Sensor Gateway **");
   ESP_LOGI(TAG, "*******************************");
   ESP_LOGI(TAG, "");
   ESP_LOGI(TAG, "Main:");
   ESP_LOGI(TAG, " - IDF version   : %s", esp_get_idf_version());
   ESP_LOGI(TAG, " - Free memory   : %d bytes", esp_get_free_heap_size());

   xTaskCreatePinnedToCore(executionContext, "exe-ctx-main", 20480, nullptr, 10, nullptr, 1);

   ESP_LOGI(TAG, "Main exit await: free stack = %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));
   sMainExitLatch.wait();
}
