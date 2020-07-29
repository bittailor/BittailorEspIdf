//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::RepeatingAlarm
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_RepeatingAlarm__h
#define INC__Bt_AlarmClock_RepeatingAlarm__h

#include <vector>
#include <tuple>
#include <inttypes.h>

#include <date/date.h>

#include <Bt/Core/I_Time.h>
#include <Bt/Core/I_Timezone.h>

#include <Bt/AlarmClock/I_Alarm.h>

namespace Bt {
namespace AlarmClock {

class RepeatingAlarm : public I_Alarm
{
   public:

      RepeatingAlarm(Core::I_Time& pTime, Core::I_Timezone& pTimezone, uint8_t pHour, uint8_t pMinute, const std::vector<date::weekday>& pDays);
      RepeatingAlarm(const RepeatingAlarm&) = delete;
      RepeatingAlarm& operator=(const RepeatingAlarm&) = delete;
      ~RepeatingAlarm();

      Check check() override;
      std::chrono::system_clock::time_point nextInLocal() override;
      std::chrono::seconds seconds() override;

   private:
      void enable();
      
      Core::I_Time& mTime;
      Core::I_Timezone& mTimezone;
      uint8_t mHour;
      uint8_t mMinute;
      std::vector<date::weekday> mDays;
      std::chrono::system_clock::time_point mNextInUtc;

};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_RepeatingAlarm__h

