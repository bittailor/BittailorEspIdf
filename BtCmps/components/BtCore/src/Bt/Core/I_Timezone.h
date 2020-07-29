//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Core::I_Timezone
//
//*************************************************************************************************

#ifndef INC__Bt_Core_I_Timezone__h
#define INC__Bt_Core_I_Timezone__h

#include <chrono>
#include <date/date.h>

namespace Bt {
namespace Core {

class I_Timezone {
   public:
      typedef date::time_of_day<std::chrono::system_clock::duration> time_of_day; 
      typedef date::year_month_day year_month_day; 
      struct TimeTuple {
            year_month_day date;
            time_of_day time;      
      };

      virtual ~I_Timezone() {}

      virtual std::chrono::system_clock::time_point toLocal(std::chrono::system_clock::time_point pUtc) = 0;
      virtual std::chrono::system_clock::time_point toUTC(std::chrono::system_clock::time_point pLocal) = 0;
      
      virtual TimeTuple localTime(std::chrono::system_clock::time_point pUtc) = 0;

      static TimeTuple toTimeTuple(std::chrono::system_clock::time_point pTimePoint) {
         auto datePoint = date::floor<date::days>(pTimePoint);
         auto timeOfDay = date::make_time(pTimePoint - datePoint);
         return TimeTuple{date::year_month_day{datePoint}, timeOfDay};
      }
};

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_I_Timezone__h

