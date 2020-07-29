//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::AlarmController
//
//*************************************************************************************************

#include "Bt/AlarmClock/AlarmController.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "Bt/AlarmClock/Tag.h"

namespace Bt {
namespace AlarmClock {

namespace {
   std::map<std::string, date::weekday> sWeekdayLookup{
      {"sun", date::literals::sun},
      {"mon", date::literals::mon},
      {"tue", date::literals::tue},
      {"wed", date::literals::wed},
      {"thu", date::literals::thu},
      {"fri", date::literals::fri},
      {"sat", date::literals::sat},
   };   
}

AlarmController::AlarmController(Concurrency::I_ExecutionContext& pExecutionContext, Core::I_Time& pTime, Core::I_Timezone& pTimezone, I_AlarmActor& pAlarmActor)
: mExecutionContext(pExecutionContext), mTime(pTime), mTimezone(pTimezone), mAlarmActor(pAlarmActor), mMinuteUpdateSubscription(pExecutionContext,[this](auto pEvent){
   checkLoop();   
}) {
}

AlarmController::~AlarmController() {
}

bool AlarmController::configure(JsonVariant& pJson) {
   mAlarms.clear();
   
   JsonArray repeatingAlarms = pJson["repeatingAlarms"];
   for (JsonObject repeatingAlarm  : repeatingAlarms) {
      if(repeatingAlarm["e"].as<bool>()) {
         uint8_t hour =  repeatingAlarm["h"];
         uint8_t minute =  repeatingAlarm["m"];
         std::vector<date::weekday> weekdays;
         JsonArray days = repeatingAlarm["days"];
         for (const char* day : days){
            weekdays.push_back(sWeekdayLookup[day]);   
         }
         std::shared_ptr<RepeatingAlarm> alarm = std::make_shared<RepeatingAlarm>(mTime, mTimezone, hour, minute, weekdays);
         mAlarms.push_back(alarm);
      }
   }   
   ESP_LOGI(TAG, "AlarmController load : %zu Repeating Alarms", repeatingAlarms.size());

   JsonArray onceAlarms = pJson["onceAlarms"];
   for (JsonObject onceAlarm  : onceAlarms) {
      if(onceAlarm["e"].as<bool>()) {
         uint8_t hour =  onceAlarm["h"];
         uint8_t minute =  onceAlarm["m"];
         uint8_t pDay =  onceAlarm["D"];
         uint8_t pMonth =  onceAlarm["M"];
         uint32_t pYear =  onceAlarm["Y"];
         
         std::shared_ptr<OnceAlarm> alarm = std::make_shared<OnceAlarm>(mTime, mTimezone, hour, minute, pDay, pMonth, pYear);
         if(alarm->check().mKeep) {
             mAlarms.push_back(alarm);
         }        
      }
   }   
   ESP_LOGI(TAG, "AlarmController load : %zu Once Alarms", onceAlarms.size());
   
   return true;
}

Core::Result<std::chrono::system_clock::time_point> AlarmController::nextInLocal() {
   auto nextAlarm = std::min_element(std::begin(mAlarms), std::end(mAlarms),
      [](const std::shared_ptr<I_Alarm>  &lhs, const std::shared_ptr<I_Alarm>  &rhs){
      return lhs->nextInLocal() < rhs->nextInLocal();
   });
   if(nextAlarm == std::end(mAlarms)) {
      return Core::Result<std::chrono::system_clock::time_point>(); 
   }
   return (*nextAlarm)->nextInLocal();
}

void AlarmController::checkLoop() {
   std::list<std::shared_ptr<I_Alarm>> toRemove;
   for(auto alarm : mAlarms) {
      auto check = alarm->check();
      if (check.mTrigger) {
         activateAlarm(alarm);
      }
      if (!check.mKeep) {
         toRemove.push_back(alarm); 
      }
   }
   for (auto alarm : toRemove)
   {
      mAlarms.remove(alarm);   
   }
}

void AlarmController::activateAlarm(std::shared_ptr<I_Alarm> pAlarm) {
   mAlarmActor.activateAlarm(pAlarm);
}

} // namespace AlarmClock
} // namespace Bt

