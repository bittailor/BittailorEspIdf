//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>
#include <date/date.h>

#include "Bt/Core/Timezone.h"
#include "Bt/Core/TimeStub.h"

using namespace std::chrono_literals;
using namespace date;

namespace Bt {
namespace Core {

TEST_CASE( "TimezoneTest", "[TimezoneTest]" ) {
   TimeStub mTime;
   Timezone mTimezone;

   SECTION("test standard time") {
      auto utc = sys_days{2019_y/November/25} + 10h + 30min;
      auto local = mTimezone.localTime(utc);
      REQUIRE(local.time.hours() == 11h);
      REQUIRE(local.time.minutes() == 30min);
   }

   SECTION("test daylight saving time") {
      auto utc = sys_days{2019_y/July/25} + 10h + 30min;
      auto local = mTimezone.localTime(utc);
      REQUIRE(local.time.hours() == 12h);
      REQUIRE(local.time.minutes() == 30min);
   }


   SECTION("test one") {
      auto utc = mTime.now();
      auto local = mTimezone.toLocal(utc);
      auto rounttrip = mTimezone.toUTC(local);
      REQUIRE(utc == rounttrip);
      REQUIRE(utc != local);

      auto tm = std::chrono::system_clock::to_time_t(utc);
      auto gm = gmtime(&tm);
      
      auto dp = date::floor<date::days>(utc);
      date::year_month_day ymdUtc{dp};
      date::time_of_day<std::chrono::seconds> time{date::floor<std::chrono::seconds>(utc-dp)};
      
      REQUIRE(static_cast<unsigned>(ymdUtc.day()) == gm->tm_mday); 
      REQUIRE(static_cast<unsigned>(ymdUtc.day()) == gm->tm_mday); 
      REQUIRE(static_cast<unsigned>(ymdUtc.day()) == gm->tm_mday);

      REQUIRE(time.hours().count() == gm->tm_hour); 
      REQUIRE(time.minutes().count() == gm->tm_min); 
      REQUIRE(time.seconds().count() == gm->tm_sec); 

   }   
   
}

} // namespace Core
} // namespace Bt

