//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::Time
//
//*************************************************************************************************

#include "Bt/Core/Time.h"

#include <esp_timer.h>

namespace Bt {
namespace Core {

Time::Time() {

}

Time::~Time() {

}


uint32_t Time::milliseconds() {
   return static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
}

uint32_t Time::microseconds() {
   return static_cast<uint32_t>(esp_timer_get_time());
}

Time::TimePoint Time::now() {
   return std::chrono::system_clock::now();
}

} // namespace Core
} // namespace Bt
