//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::ConfigurationController
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_ConfigurationController__h
#define INC__Bt_AlarmClock_ConfigurationController__h

#include <functional>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include <ArduinoJson.h>

namespace Bt {
namespace AlarmClock {

class ConfigurationController {
  public:
   typedef std::function<std::tuple<bool, std::string>(JsonVariant&)> Handler;

   ConfigurationController();
   ConfigurationController(const ConfigurationController&) = delete;
   ConfigurationController& operator=(const ConfigurationController&) = delete;
   ~ConfigurationController();

   ConfigurationController& add(const std::string& pKey, Handler pHandler, bool pImmediateApply = true);

   bool configure(const std::string& pKey);

   std::tuple<bool, std::string> handle(const std::string& pKey, JsonVariant& pJson);

   std::shared_ptr<JsonDocument> loadConfiguration(const std::string& pKey);
   bool removeConfiguration(const std::string& pKey);

  private:
   bool storeConfiguration(const std::string& pKey, JsonVariant& pJson);
   std::shared_ptr<JsonDocument> loadFrom(std::istream& stream);

   std::string mDefaultPath;
   std::string mStoragePath;
   std::map<std::string, std::function<std::tuple<bool, std::string>(JsonVariant&)>> mHandlers;
};

}  // namespace AlarmClock
}  // namespace Bt

#endif  // INC__Bt_AlarmClock_ConfigurationController__h
