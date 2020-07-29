//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::OnceAlarm
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_OnceAlarm__h
#define INC__Bt_AlarmClock_OnceAlarm__h

#include <inttypes.h>

#include <date/date.h>

#include <Bt/Core/I_Time.h>
#include <Bt/Core/I_Timezone.h>

#include <Bt/AlarmClock/I_Alarm.h>

namespace Bt {
namespace AlarmClock {

class OnceAlarm : public I_Alarm
{
   public:
   
      OnceAlarm(Core::I_Time& pTime, Core::I_Timezone& pTimezone, uint8_t pHour, uint8_t pMinute, uint8_t pDay, uint8_t pMonth, uint32_t pYear);
      OnceAlarm(const OnceAlarm&) = delete;
      OnceAlarm& operator=(const OnceAlarm&) = delete;
      ~OnceAlarm();

      Check check();
      std::chrono::system_clock::time_point nextInLocal();
      std::chrono::seconds seconds();

   private:
      Core::I_Time& mTime; 
      Core::I_Timezone& mTimezone;
      std::chrono::system_clock::time_point mNextInUtc;
      
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_OnceAlarm__h

