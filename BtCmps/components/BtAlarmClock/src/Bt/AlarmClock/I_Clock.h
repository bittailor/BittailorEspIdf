//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::AlarmClock::I_Clock
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_I_Clock__h
#define INC__Bt_AlarmClock_I_Clock__h

#include <Bt/Core/I_Timezone.h>

namespace Bt {
namespace AlarmClock {

class I_Clock {
   public:
      struct SecondUpdate{};
      struct MinuteUpdate{};
      struct HourUpdate{};
      
      virtual ~I_Clock() {}

      virtual Core::I_Timezone::TimeTuple local() = 0;

      //virtual void function() = 0;
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_I_Clock__h

