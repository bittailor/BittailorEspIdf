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

   Bt::Bluetooth::BleController bleController;
   
   Bt::Devices::Xiaomi::DeviceFactory mXiaomiDeviceFactory;
   Bt::Devices::Xiaomi::HumiditySensor::registerAtFactory(mXiaomiDeviceFactory, bleController) ;
   Bt::Devices::Xiaomi::BleDiscoveryAgent xiaomiBleDiscoveryAgent(bleController);

   //std::vector<std::shared_ptr<Bt::Devices::Xiaomi::I_Device>> xiaomiDevices;
   std::queue<std::shared_ptr<Bt::Devices::Xiaomi::I_Device>> xiaomiDevicesToConnect;
   std::queue<std::shared_ptr<Bt::Devices::Xiaomi::I_Device>> xiaomiDevicesConnected;


   std::function<void()> connectNextDevice = [&mainExecutionContext,&xiaomiDevicesToConnect,&xiaomiDevicesConnected,&connectNextDevice](){
      if(xiaomiDevicesToConnect.empty()){
         return;
      }   
      auto device = xiaomiDevicesToConnect.front();
      if(device->connect()){
         xiaomiDevicesConnected.push(device);
         xiaomiDevicesToConnect.pop();  
      }
      mainExecutionContext.callOnce(std::chrono::seconds(10),connectNextDevice);
   };

   int counter = 0;
   std::function<void()> startScan = [&counter, &time, &mainExecutionContext, &mqttController, &mXiaomiDeviceFactory, &xiaomiDevicesToConnect, &connectNextDevice, &xiaomiBleDiscoveryAgent](){
      counter++;
      ESP_LOGI(TAG, "startScan counter = %d", counter);
      if(counter < 2) {
         return;
      }
      ESP_LOGI(TAG, "start discover");
      xiaomiBleDiscoveryAgent.discoverBleDevices(std::chrono::seconds(20),
         [&time, &mainExecutionContext, &mqttController, &mXiaomiDeviceFactory, &xiaomiDevicesToConnect, &connectNextDevice](auto&& pDevices){
            ESP_LOGI(TAG, "discover completed %zu devices", pDevices.size());
            mainExecutionContext.call([&time, &mainExecutionContext, &mqttController, &mXiaomiDeviceFactory, &xiaomiDevicesToConnect, &connectNextDevice, pDevices](){
               // int delay = 5;
               for (auto device : pDevices)
               {                
                  auto xiaomiDevice = mXiaomiDeviceFactory.createDevice(device);
                  xiaomiDevicesToConnect.push(xiaomiDevice);
                  
                  // mainExecutionContext.callOnce(std::chrono::seconds(delay),[device,xiaomiDevice](){
                  //    printf("\n");
                  //    printf("\n");
                  //    ESP_LOGI(TAG, "connect to => %s", device->address().toString().c_str());           
                  //    xiaomiDevice->connect(); 
                  // });
                  // delay += 10;
               }
               mainExecutionContext.callOnce(std::chrono::seconds(5),connectNextDevice);
               
               
               
               // if(pDevices.size() > 0) {
               //    auto xiaomiDevice = mXiaomiDeviceFactory.createDevice(pDevices[0]);
               //    xiaomiDevices.push_back(xiaomiDevice);
               //    xiaomiDevice->connect();
               // }
               
               /*
               {
                  DynamicJsonDocument doc(1024);
                  Bt::Core::StringBuilder<200> builder;          
                  for (auto device : pDevices)
                  {
                     builder.reset().hexencode(device->serviceData());
                     doc[device->address().toString()].set(builder.c_str()); 
                  } 
                  mqttController.publish("bittailor/home/sensor/mijia/discover", doc.as<std::string>());
               }
               {
                  DynamicJsonDocument doc(1024);
                  auto heap = doc.createNestedObject("heap");          
                  heap["free"].set(esp_get_free_heap_size());
                  heap["min"].set(esp_get_minimum_free_heap_size());
                  heap["block"].set(heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT));               
                  doc["uptime"].set(time.milliseconds());
                  mqttController.publish("bittailor/home/gateway/health", doc.as<std::string>());
               }
               
               mainExecutionContext.callOnce(std::chrono::seconds(2), startScan); 
               */

            });
         });

      /*
      auto discoveryAgent = new Bt::Bluetooth::BleDeviceDiscoveryAgent(
         [&mainExecutionContext, &mqttController](auto pDeviceInfo){
            if(!pDeviceInfo->serviceUuid().isEmpty() && pDeviceInfo->serviceUuid() == 0xfe95 ) {
               ESP_LOGI(TAG, "DeviceInfo with serviceUuid %s address %s",
                  pDeviceInfo->serviceUuid().toString().c_str(),
                  pDeviceInfo->address().toString().c_str());   
               mainExecutionContext.call([&mqttController, pDeviceInfo](){
                  mqttController.publish("bittailor/home/sensor/mijia/discover", pDeviceInfo->address().toString());    
               }); 
            }
         },
         [&mainExecutionContext, &startScan](auto devices){
            ESP_LOGI(TAG, "discover completed %zu devices", devices.size());
            mainExecutionContext.callOnce(std::chrono::seconds(2), startScan);   
         }
      );

      discoveryAgent->interval(std::chrono::milliseconds(35));
      discoveryAgent->window(std::chrono::milliseconds(20));
      discoveryAgent->duplicateFilter(true);
      discoveryAgent->start(std::chrono::seconds(20));
      */
   };


   Bt::Events::Subscription<Bt::Bluetooth::I_BleController::Synced> onBleSynced(mainExecutionContext,[&startScan](auto pEvent){
      ESP_LOGI(TAG, "onBleSynced");
      startScan();
   });
   Bt::Events::Subscription<Bt::Protocols::I_MqttController::Connected> onMqttConnected(mainExecutionContext,[&startScan](auto pEvent){
      ESP_LOGI(TAG, "onMqttConnected");
      startScan();
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
