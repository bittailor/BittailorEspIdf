//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::RepeatingAlarm
//
//*************************************************************************************************

#include "Bt/AlarmClock/RepeatingAlarm.h"


#include <algorithm>

#include "Bt/AlarmClock/Tag.h"

using namespace date;
using namespace std::chrono;

namespace Bt {
namespace AlarmClock {

RepeatingAlarm::RepeatingAlarm(Core::I_Time& pTime, Core::I_Timezone& pTimezone, uint8_t pHour, uint8_t pMinute, const std::vector<date::weekday>& pDays)
: mTime(pTime), mTimezone(pTimezone), mHour(pHour), mMinute(pMinute), mDays(pDays)  {
   enable();
}

RepeatingAlarm::~RepeatingAlarm() {

}

 RepeatingAlarm::Check RepeatingAlarm::check() {
   auto now = mTime.now(); 
   if (now < mNextInUtc) {
      return {false, true};
   }   
   std::chrono::minutes diff =  duration_cast<std::chrono::minutes>(now - mNextInUtc); 
   enable();
   if (diff > std::chrono::minutes(cMaxDelayInMinutesToStillTriggerAlarm)) {
      ESP_LOGW(TAG, "missed alarm time by more than %" PRIu32 "minutes => will not activate alarm", cMaxDelayInMinutesToStillTriggerAlarm);
      return {false, true};
   }
   return {true, true};
}

std::chrono::system_clock::time_point RepeatingAlarm::nextInLocal() {
   return mTimezone.toLocal(mNextInUtc);
}

std::chrono::seconds RepeatingAlarm::seconds() {
   return duration_cast<std::chrono::seconds>(mNextInUtc - mTime.now());
}

void RepeatingAlarm::enable() {
   
   auto now = mTime.now(); 
   auto local = mTimezone.localTime(now);

   auto next = sys_days{local.date} + hours(mHour) + minutes(mMinute);

   if ( hours(mHour) < local.time.hours() ||
        (hours(mHour) == local.time.hours() && minutes(mMinute) <= local.time.minutes())) {
      next = next + days(1);
   }
   
   if (!mDays.empty()) {
      while(std::find(std::begin(mDays), std::end(mDays), weekday{floor<days>(next)}) ==  std::end(mDays)) {
         next = next + days(1);   
      }
   }
   mNextInUtc = mTimezone.toUTC(next);
}

} // namespace AlarmClock
} // namespace Bt

