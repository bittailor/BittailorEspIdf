//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Core::I_Time
//
//*************************************************************************************************

#ifndef INC__Bt_Core_I_Time__h
#define INC__Bt_Core_I_Time__h

#include <cstdint>
#include <chrono>

namespace Bt {
namespace Core {

class I_Time {
   public:
      typedef std::chrono::system_clock::time_point TimePoint;
      struct TimeSyncCompleted{};

      virtual ~I_Time() {}

      virtual uint32_t milliseconds() = 0;
      virtual uint32_t microseconds() = 0;
      virtual TimePoint now() = 0;
};

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_I_Time__h

