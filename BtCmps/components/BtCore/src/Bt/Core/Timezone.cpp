//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::Timezone
//
//*************************************************************************************************

#include "Bt/Core/Timezone.h"

#include <date/date.h>

using namespace std::chrono_literals;
using namespace date;


namespace Bt {
namespace Core {
namespace {

    constexpr auto CET  = +1h; // Central European Time
    constexpr auto CEST = +2h; // Central European Summer Time

   std::chrono::hours utcOffsetCentralEuropeanTime(std::chrono::system_clock::time_point pTimepoint) {
      const auto y = year_month_day{floor<days>(pTimepoint)}.year();
      const auto begin = sys_days{Sunday[last]/March/y} + 2h - CET; // EDT begins at this UTC time
      const auto end   = sys_days{Sunday[last]/October/y} + 2h - CEST; // EST begins at this UTC time
      if (pTimepoint < begin || end <= pTimepoint)
         return CET;
      return CEST;
   }
}

/*
struct dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600}; // Central European Summer Time = UTC/GMT +2 hours
struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};       // Central European Time = UTC/GMT +1 hour
*/

Timezone::Timezone() {

}

Timezone::~Timezone() {

}

std::chrono::system_clock::time_point Timezone::toLocal(std::chrono::system_clock::time_point pUtc) {
   return pUtc + utcOffsetCentralEuropeanTime(pUtc);
}

std::chrono::system_clock::time_point Timezone::toUTC(std::chrono::system_clock::time_point pLocal) {
   return pLocal - utcOffsetCentralEuropeanTime(pLocal);
}

Timezone::TimeTuple Timezone::localTime(std::chrono::system_clock::time_point pUtc) {
   return I_Timezone::toTimeTuple(toLocal(pUtc));
   /*
   auto datePoint = date::floor<date::days>(local);
   auto timeOfDay = make_time(local - datePoint);
   return TimeTuple{date::year_month_day{datePoint}, timeOfDay};
   */
}

} // namespace Core
} // namespace Bt

