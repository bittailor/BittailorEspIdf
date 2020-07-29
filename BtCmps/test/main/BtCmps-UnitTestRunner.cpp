#include <esp_log.h>
#include <esp_system.h>

#include <iostream>
#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

constexpr const char* TAG = "BtCoreUnitTest";

int catchMain(int argc, const char* argv[] ) {
    Catch::Session session;
    int result = session.run(argc, argv);
    std::cout <<  std::endl;
    if(result == 0) {
    	std::cout << "************" << std::endl;
    	std::cout << "** PASSED **" << std::endl;
    	std::cout << "************" << std::endl;
    } else {
    	std::cout << "!!!!!!!!!!!!" << std::endl;
    	std::cout << "!! FAILED !!" << std::endl;
    	std::cout << "!!!!!!!!!!!!" << std::endl;
    }
    std::cout <<  std::endl;
    return result;
}

constexpr const char* resetReasonText(esp_reset_reason_t resetReason) {
   switch(resetReason) {
      case ESP_RST_UNKNOWN:  return "Reset reason can not be determined";
      case ESP_RST_POWERON:  return "Reset due to power-on event";
      case ESP_RST_EXT:      return "Reset by external pin (not applicable for ESP32)";
      case ESP_RST_SW:       return "Software reset via esp_restart";
      case ESP_RST_PANIC:    return "Software reset due to exception/panic";
      case ESP_RST_INT_WDT:  return "Reset (software or hardware) due to interrupt watchdog";
      case ESP_RST_TASK_WDT: return "Reset due to task watchdog";
      case ESP_RST_WDT:      return "Reset due to other watchdogs";
      case ESP_RST_DEEPSLEEP:return "Reset after exiting deep sleep mode";
      case ESP_RST_BROWNOUT: return "Brownout reset (software or hardware)";
      case ESP_RST_SDIO:     return "Reset over SDIO";
   }
   return "?";
}

extern "C" void app_main(void)
{
   ESP_LOGI(TAG, "[APP] Startup..");
   ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
   esp_log_level_set("*", ESP_LOG_INFO);

   esp_reset_reason_t resetReason = esp_reset_reason();
   ESP_LOGI(TAG, "[APP] resetReason: %s", resetReasonText(resetReason));

   if(resetReason == ESP_RST_PANIC) {
      std::cout << "!!!!!!!!!!!!" << std::endl;
      std::cout << "!! ERROR  !!" << std::endl;
      std::cout << "!!!!!!!!!!!!" << std::endl;
      return;
   }

	const char* argv[] = {"firmware"/*, "--success"*/};
	catchMain(sizeof(argv)/sizeof(argv[0]), argv);
}



