//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::AlarmController
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_AlarmController__h
#define INC__Bt_AlarmClock_AlarmController__h

#include <list>
#include <memory>

#include <ArduinoJson.h>

#include <Bt/Core/Logging.h>
#include <Bt/Core/Result.h>
#include <Bt/Concurrency/I_ExecutionContext.h>
#include <Bt/Events/Events.h>

#include "Bt/AlarmClock/Tag.h"
#include "Bt/AlarmClock/I_Clock.h"
#include "Bt/AlarmClock/I_AlarmActor.h"
#include "Bt/AlarmClock/RepeatingAlarm.h"
#include "Bt/AlarmClock/OnceAlarm.h"

namespace Bt {
namespace AlarmClock {

class AlarmController
{
   public:
      static constexpr size_t JSON_DOCUMENT_CAPACITY = 5*1024;

      AlarmController(Concurrency::I_ExecutionContext& pExecutionContext, Core::I_Time& pTime, Core::I_Timezone& pTimezone, I_AlarmActor& pAlarmActor);
      AlarmController(const AlarmController&) = delete;
      AlarmController& operator=(const AlarmController&) = delete;
      ~AlarmController();

      template<typename Input>
      bool load(Input& pInput) {
         DynamicJsonDocument doc(JSON_DOCUMENT_CAPACITY);
         auto result = deserializeJson(doc, pInput);
         if(result != DeserializationError::Ok) {
            ESP_LOGE(TAG, "alarms parsing failed: %s", result.c_str());
            return false;
         }
         JsonVariant json = doc.as<JsonVariant>();
         return configure(json);
      }

      template<typename Input>
      bool load(Input* pInput) {
         DynamicJsonDocument doc(JSON_DOCUMENT_CAPACITY);
         auto result = deserializeJson(doc, pInput);
         if(result != DeserializationError::Ok) {
            ESP_LOGE(TAG, "alarms parsing failed: %s", result.c_str());
            return false;
         }
         JsonVariant json = doc.as<JsonVariant>();
         return configure(json);
      }

      bool configure(JsonVariant& pJson);

      Core::Result<std::chrono::system_clock::time_point> nextInLocal();

   private:

      void activateAlarm(std::shared_ptr<I_Alarm> pAlarm);
      void checkLoop();

      Concurrency::I_ExecutionContext& mExecutionContext;
      Core::I_Time& mTime; 
      Core::I_Timezone& mTimezone;  
      I_AlarmActor& mAlarmActor;
      Events::Subscription<I_Clock::MinuteUpdate> mMinuteUpdateSubscription;
      std::list<std::shared_ptr<I_Alarm>> mAlarms;
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_AlarmController__h

