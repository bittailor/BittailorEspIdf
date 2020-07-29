//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::OnceAlarm
//
//*************************************************************************************************

#include "Bt/AlarmClock/OnceAlarm.h"

#include "Bt/AlarmClock/Tag.h"

using namespace date;
using namespace std::chrono;

namespace Bt {
namespace AlarmClock {

OnceAlarm::OnceAlarm(Core::I_Time& pTime, Core::I_Timezone& pTimezone, uint8_t pHour, uint8_t pMinute, uint8_t pDay, uint8_t pMonth, uint32_t pYear)
: mTime(pTime), mTimezone(pTimezone), mNextInUtc(mTimezone.toUTC(sys_days{day{pDay}/pMonth/pYear} + hours(pHour) + minutes(pMinute)))  {
   
}

OnceAlarm::~OnceAlarm() {

}

OnceAlarm::Check OnceAlarm::check() {
   auto now = mTime.now(); 
   if (now < mNextInUtc) {
      return {false, true};
   }   
   std::chrono::minutes diff =  duration_cast<std::chrono::minutes>(now - mNextInUtc); 
   if (diff > std::chrono::minutes(cMaxDelayInMinutesToStillTriggerAlarm)) {
      ESP_LOGW(TAG, "missed alarm time by more than %" PRIu32 "minutes => will not activate alarm", cMaxDelayInMinutesToStillTriggerAlarm);
      return {false, false};
   }
   return {true, false};
}

std::chrono::system_clock::time_point OnceAlarm::nextInLocal() {
   return mTimezone.toLocal(mNextInUtc);
}

std::chrono::seconds OnceAlarm::seconds() {
   return duration_cast<std::chrono::seconds>(mNextInUtc - mTime.now());
}

} // namespace AlarmClock
} // namespace Bt

