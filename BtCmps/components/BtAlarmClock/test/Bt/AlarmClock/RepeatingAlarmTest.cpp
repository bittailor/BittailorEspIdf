//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>

#include "Bt/Core/Timezone.h"
#include "Bt/Core/TimeMock.h"
#include "Bt/AlarmClock/RepeatingAlarm.h"

using namespace std::chrono_literals;
using namespace date;

namespace Bt {
namespace AlarmClock {

TEST_CASE( "AlarmTest", "[AlarmTest]" ) {
   Core::TimeMock timeMock;
   Core::Timezone timezone;
   

   SECTION("some day stuff") {
      auto sunFromLiteral = date::literals::sun;
      auto sunFromIndex = date::weekday(0);
      REQUIRE(sunFromLiteral == sunFromIndex);
   } 

   SECTION("an alarm 5:30 from tuesday to friday") {
      timeMock.mNow =  sys_days{2019_y/October/24} + 10h + 44min;
      RepeatingAlarm alarm{timeMock, timezone, 5, 30, {date::literals::wed, date::literals::thu, date::literals::fri}};
      REQUIRE(alarm.nextInLocal() == (sys_days{2019_y/October/25} + 5h + 30min));
      timeMock.mNow =  sys_days{2019_y/October/25} + 3h + 29min;
      REQUIRE_FALSE(alarm.check().mTrigger);
      timeMock.mNow =  sys_days{2019_y/October/25} + 3h + 31min;
      REQUIRE(alarm.check().mTrigger);
      REQUIRE(alarm.nextInLocal() == (sys_days{2019_y/October/30} + 5h + 30min));
      timeMock.mNow =  sys_days{2019_y/October/30} + 4h + 29min;
      REQUIRE_FALSE(alarm.check().mTrigger);
      timeMock.mNow =  sys_days{2019_y/October/30} + 4h + 31min;
      REQUIRE(alarm.check().mTrigger);
   }

   SECTION("an alarm on wed and sun") {
      timeMock.mNow =  sys_days{2019_y/July/25} + 13h + 30min;
      RepeatingAlarm alarm{timeMock, timezone, 8, 30, {date::literals::sun, date::literals::wed}};
      REQUIRE(alarm.nextInLocal() == (sys_days{2019_y/July/28} + 8h + 30min));
      timeMock.mNow =  sys_days{2019_y/July/28} + 6h + 29min;
      REQUIRE_FALSE(alarm.check().mTrigger);
      timeMock.mNow =  sys_days{2019_y/July/28} + 6h + 30min;
      REQUIRE(alarm.check().mTrigger);
      REQUIRE(alarm.nextInLocal() == (sys_days{2019_y/July/31} + 8h + 30min));
   }

   SECTION("alarm just not missed by MaxDelay gap") {
      timeMock.mNow =  sys_days{2019_y/July/25} + 13h + 30min;
      RepeatingAlarm alarm{timeMock, timezone, 8, 30, {date::literals::fri}};
      REQUIRE(alarm.nextInLocal() == (sys_days{2019_y/July/26} + 8h + 30min));
      timeMock.mNow =  sys_days{2019_y/July/26} + 6h + 29min;
      REQUIRE_FALSE(alarm.check().mTrigger);
      timeMock.mNow =  sys_days{2019_y/July/26} + 6h + 30min + std::chrono::minutes(RepeatingAlarm::cMaxDelayInMinutesToStillTriggerAlarm);
      REQUIRE(alarm.check().mTrigger);
      REQUIRE(alarm.nextInLocal() == (sys_days{2019_y/August/2} + 8h + 30min));
   }


   SECTION("alarm just missed by MaxDelay gap") {
      timeMock.mNow =  sys_days{2019_y/July/25} + 13h + 30min;
      RepeatingAlarm alarm{timeMock, timezone, 8, 30, {date::literals::fri}};
      REQUIRE(alarm.nextInLocal() == (sys_days{2019_y/July/26} + 8h + 30min));
      timeMock.mNow =  sys_days{2019_y/July/26} + 6h + 29min;
      REQUIRE_FALSE(alarm.check().mTrigger);
      timeMock.mNow =  sys_days{2019_y/July/26} + 6h + 30min + std::chrono::minutes(RepeatingAlarm::cMaxDelayInMinutesToStillTriggerAlarm) + 1min;
      REQUIRE_FALSE(alarm.check().mTrigger);
      REQUIRE(alarm.nextInLocal() == (sys_days{2019_y/August/2} + 8h + 30min));
   }


}

} // namespace AlarmClock
} // namespace Bt

