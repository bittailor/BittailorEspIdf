//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::ConfigurationController
//
//*************************************************************************************************

#include "Bt/AlarmClock/ConfigurationController.h"

#include <fstream>
#include <memory>

#include "Bt/AlarmClock/Tag.h"

namespace Bt {
namespace AlarmClock {

ConfigurationController::ConfigurationController()
: mDefaultPath("/default"), mStoragePath("/config") {

}

ConfigurationController::~ConfigurationController() {

}

ConfigurationController& ConfigurationController::add(const std::string& pKey, Handler pHandler, bool pImmediateApply) {
   mHandlers.insert({pKey,pHandler});
   if(true) {
      if(!configure(pKey)) {
         ESP_LOGE(TAG, "immediate configuration failed for %s", pKey.c_str());   
      }
   }
   return *this;
}

bool ConfigurationController::configure(const std::string& pKey) {
   auto iter = mHandlers.find(pKey);
   if (iter == std::end(mHandlers)) {
      ESP_LOGE(TAG, "no configuration handler for %s", pKey.c_str());
      return false;
   }
   auto configuration = loadConfiguration(pKey);
   if(!configuration) {
      ESP_LOGE(TAG, "no configuration for %s", pKey.c_str());
      return false;   
   }
   JsonVariant json = configuration->as<JsonVariant>();
   auto result = iter->second(json);
   if(!std::get<0>(result)) {
      ESP_LOGE(TAG, "configuration for %s failed: %s", pKey.c_str(), std::get<1>(result).c_str());
      return false;
   }
   return true;
}

std::tuple<bool, std::string> ConfigurationController::handle(const std::string& pKey, JsonVariant& pJson) {
   auto iter = mHandlers.find(pKey);
   if (iter == std::end(mHandlers)) {
      ESP_LOGE(TAG, "no configuration handler for %s", pKey.c_str());
      return std::make_tuple(false, std::string("no configuration handler for ") + pKey.c_str()) ;
   } 
   auto result = iter->second(pJson);
   if(std::get<0>(result)) {
      storeConfiguration(pKey, pJson);   
   }  
   return result;
}

std::shared_ptr<JsonDocument> ConfigurationController::loadConfiguration(const std::string& pKey) {
   std::fstream storageFile(mStoragePath + "/" + pKey + ".json", std::ifstream::in);
   if(storageFile) {
      ESP_LOGI(TAG, "load %s configuration from storage", pKey.c_str());
      return loadFrom(storageFile);
   }

   std::fstream defaultFile(mDefaultPath + "/" + pKey + ".json", std::ifstream::in);
   if(defaultFile) {
      ESP_LOGI(TAG, "load %s configuration from default", pKey.c_str());
      return loadFrom(defaultFile); 
   }
   ESP_LOGE(TAG, "no configuration found for %s", pKey.c_str());
   return nullptr;
}

bool ConfigurationController::removeConfiguration(const std::string& pKey) {
   auto iter = mHandlers.find(pKey);
   if (iter == std::end(mHandlers)) {
      ESP_LOGE(TAG, "no configuration handler for %s", pKey.c_str());
      return false;
   }
   if (std::remove((mStoragePath + "/" + pKey + ".json").c_str()) != 0) {
      ESP_LOGW(TAG, "remove configuration from storage for %s failed", pKey.c_str());
   }
   return configure(pKey);  
}

bool ConfigurationController::storeConfiguration(const std::string& pKey, JsonVariant& pJson) {
   std::fstream storageFile(mStoragePath + "/" + pKey + ".json", std::ifstream::out);
   if (!storageFile) {
      ESP_LOGE(TAG, "failed to store file for %s", pKey.c_str());
      return false;
   }
   serializeJson(pJson, storageFile);
   return true;
}

std::shared_ptr<JsonDocument> ConfigurationController::loadFrom(std::istream& stream) {
   auto document = std::make_shared<DynamicJsonDocument>(5*1024);  
   deserializeJson(*document, stream);
   return document;
}

} // namespace AlarmClock
} // namespace Bt

