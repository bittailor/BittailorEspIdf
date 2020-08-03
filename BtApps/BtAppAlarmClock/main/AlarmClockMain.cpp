#include <iostream>

#include <esp_log.h>
#include <esp_system.h>
#include <soc/rtc.h>
#include <nvs_flash.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <ArduinoJson.h>

#include <Bt/Core/Time.h>
#include <Bt/Core/Timezone.h>
#include <Bt/Core/StringBuilder.h>
#include <Bt/Concurrency/CountdownLatch.h>
#include <Bt/Concurrency/SchedulingExecutionContext.h>
#include <Bt/Events/Events.h>
#include <Bt/Events/DefaultEventLoop.h>
#include <Bt/Network/WiFiController.h>
#include <Bt/Peripherals/DigitalOut.h>
#include <Bt/Peripherals/InterruptPushButton.h>
#include <Bt/Peripherals/PwmOut.h>
#include <Bt/Peripherals/RgbLed.h>
#include <Bt/Protocols/SntpController.h>
#include <Bt/Protocols/MqttController.h>
#include <Bt/Storage/NvsRepository.h>
#include <Bt/Storage/VirtualFilesystem.h>

#include <Bt/AlarmClock/AlarmActor.h>
#include <Bt/AlarmClock/AlarmController.h>
#include <Bt/AlarmClock/Buzzer.h>
#include <Bt/AlarmClock/Clock.h>
#include <Bt/AlarmClock/ConfigurationController.h>
#include <Bt/AlarmClock/DisplayBacklight.h>
#include <Bt/AlarmClock/MqttConfigurationConnector.h>

constexpr const char* TAG = "BtAlarmClockApp";

Bt::Concurrency::CountdownLatch sMainExitLatch(1);

uint32_t cpuFrequency() {
   rtc_cpu_freq_config_t config;
   rtc_clk_cpu_freq_get_config(&config);
   return config.freq_mhz;
}

void dumpTaskInfo() {
   char buffer[2500];
   vTaskList(buffer);
   std::cout << "**************************************" << std::endl;
   std::cout << "Task        State Prio  Stack  Num CPU"<< std::endl;
   std::cout << "**************************************" << std::endl;
   std::cout << buffer << std::endl;
   std::cout << "**************************************" << std::endl;
   std::cout << std::endl;
   std::cout << "**************************************" << std::endl;
   vTaskGetRunTimeStats(buffer);
   std::cout << buffer << std::endl;
}


void alarmClockMain(void* pContext)
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

   Bt::Network::WiFiController wiFiController(mainExecutionContext, defaultEventLoop, CONFIG_BT_ALARM_CLOCK_WIFI_SSID, CONFIG_BT_ALARM_CLOCK_WIFI_PASSWORD);
   Bt::Protocols::SntpController sntpController(defaultEventLoop);


   Bt::Protocols::MqttController mqttController(defaultEventLoop,
                                                CONFIG_BT_ALARM_CLOCK_MQTT_URI,
                                                [](esp_mqtt_client_config_t& cfg){
                                                      cfg.client_id = CONFIG_BT_ALARM_CLOCK_MQTT_CLIENT_ID;
                                                      cfg.username = CONFIG_BT_ALARM_CLOCK_MQTT_USERNAME;
                                                      cfg.password = CONFIG_BT_ALARM_CLOCK_MQTT_PASSWORD;
                                                });


   Bt::AlarmClock::Clock clock(mainExecutionContext, time, timezone);

   Bt::Peripherals::PwmTimer buzzerPwmTimer(LEDC_TIMER_0, LEDC_HIGH_SPEED_MODE, 4, LEDC_TIMER_16_BIT);
   Bt::Peripherals::PwmOut buzzerPwm(buzzerPwmTimer, LEDC_CHANNEL_0, GPIO_NUM_4);
   Bt::AlarmClock::Buzzer buzzer(buzzerPwmTimer, buzzerPwm);

   Bt::Peripherals::PwmTimer ledsPwmTimer(LEDC_TIMER_1, LEDC_HIGH_SPEED_MODE, 12000, LEDC_TIMER_8_BIT);
   Bt::Peripherals::PwmOut red(ledsPwmTimer, LEDC_CHANNEL_1, GPIO_NUM_33);
   Bt::Peripherals::PwmOut green(ledsPwmTimer, LEDC_CHANNEL_2, GPIO_NUM_32);
   Bt::Peripherals::PwmOut blue(ledsPwmTimer, LEDC_CHANNEL_3, GPIO_NUM_22);
   Bt::Peripherals::RgbLed rgbLed(red, green, blue, true);
   Bt::AlarmClock::DisplayBacklight displayBacklight(mainExecutionContext, rgbLed);


   Bt::AlarmClock::AlarmActor alarmActor(mainExecutionContext, buzzer);
   Bt::AlarmClock::AlarmController alarmController(mainExecutionContext, time, timezone, alarmActor);

   Bt::AlarmClock::ConfigurationController configurationController;
   Bt::AlarmClock::MqttConfigurationConnector mqttConfigurationConnector(mainExecutionContext, configurationController,mqttController);

   Bt::Peripherals::InterruptPushButton resetAlarm(mainExecutionContext, GPIO_NUM_16,
                                                   [&alarmActor](auto pEvent) {
      ESP_LOGI(TAG, "ResetAlarm => %d", Bt::Core::asInteger(pEvent));
      if (pEvent == Bt::Peripherals::InterruptPushButton::Event::PRESSED) {
         alarmActor.stoppPressed();
      }
   });

   Bt::Peripherals::InterruptPushButton blueButton(mainExecutionContext,GPIO_NUM_17,
                                                   [&displayBacklight](auto pEvent) {
      if (pEvent == Bt::Peripherals::InterruptPushButton::Event::PRESSED) {
         displayBacklight.on();
      }
   });

   configurationController
      .add("alarms", [&alarmController](JsonVariant& pJson) {
         if (!alarmController.configure(pJson)) {
            return std::make_tuple(false, "invalid body");
         }
         return std::make_tuple(true, "ok");
      })
      .add("buzzer", [&buzzer](JsonVariant& pJson) {
         JsonObject buzzerConfiguration = pJson.as<JsonObject>();
         JsonVariant f = buzzerConfiguration["frequency"];
         if (f.is<double>()) {
            buzzer.configure(f.as<double>());
            return std::make_tuple(true, "ok");
         }
         return std::make_tuple(false, "invalid body");
      })
      .add("backlight", [&displayBacklight](JsonVariant& pJson) {
         JsonObject buzzerConfiguration = pJson.as<JsonObject>();

         ESP_LOGI(TAG, "backlight containsKey(hue) = %d", buzzerConfiguration.containsKey("hue"));
         ESP_LOGI(TAG, "backlight containsKey(brightness) = %d", buzzerConfiguration.containsKey("brightness"));
         ESP_LOGI(TAG, "backlight containsKey(autoOffTimeInSeconds) = %d", buzzerConfiguration.containsKey("autoOffTimeInSeconds"));

         std::string dump;
         serializeJsonPretty(pJson, dump);
         std::cout << std::endl << dump << std::endl;

         JsonVariant h = buzzerConfiguration["hue"];
         JsonVariant b = buzzerConfiguration["brightness"];
         JsonVariant t = buzzerConfiguration["autoOffTimeInSeconds"];
         if (h.is<uint32_t>() && b.is<uint32_t>() && t.is<uint32_t>()) {
            displayBacklight.configure(h.as<uint32_t>(), b.as<uint32_t>(), std::chrono::seconds(t.as<uint32_t>()));
            return std::make_tuple(true, "ok");
         }
         return std::make_tuple(false, "invalid body");
      });

   Bt::Events::Subscription<Bt::AlarmClock::I_Clock::MinuteUpdate> minuteUpdateSubscription(mainExecutionContext, [&clock, &mqttController](auto pEvent) {
      auto local = clock.local();

      ESP_LOGI(TAG, "Minute Update %02d:%02d:%02d  %02u/%02u/%4d",
               static_cast<int>(local.time.hours().count()),
               static_cast<int>(local.time.minutes().count()),
               static_cast<int>(local.time.seconds().count()),
               static_cast<unsigned>(local.date.day()),
               static_cast<unsigned>(local.date.month()),
               static_cast<int>(local.date.year()));
      ESP_LOGI(TAG, " - Free memory : %d bytes", esp_get_free_heap_size());
      ESP_LOGI(TAG, " - Free stack  : %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));

      StaticJsonDocument<500> msgJson;
      msgJson["memory"] = esp_get_free_heap_size();
      msgJson["stack"] = uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle());
      auto time = msgJson.createNestedObject("time");
      time["h"] = local.time.hours().count();
      time["m"] = local.time.minutes().count();
      time["s"] = local.time.seconds().count();
      auto date = msgJson.createNestedObject("date");
      date["y"] = static_cast<int>(local.date.year());
      date["m"] = static_cast<unsigned>(local.date.month());
      date["d"] = static_cast<unsigned>(local.date.day());
      std::string msg;
      serializeJson(msgJson, msg);
      mqttController.publish("/btAlarmClock/devOne/status", msg);

      // dumpTaskInfo();

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
   ESP_LOGI(TAG, " - CPU frequency : %d", cpuFrequency());
   ESP_LOGI(TAG, " - Free memory   : %d bytes", esp_get_free_heap_size());

   xTaskCreatePinnedToCore(alarmClockMain, "exe-ctx-main", 20480, nullptr, 10, nullptr, 1);

   ESP_LOGI(TAG, "Main exit await: free stack = %d bytes", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));
   sMainExitLatch.wait();
}
