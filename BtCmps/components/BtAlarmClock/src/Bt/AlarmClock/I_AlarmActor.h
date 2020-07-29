//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::AlarmClock::I_AlarmActor
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_I_AlarmActor__h
#define INC__Bt_AlarmClock_I_AlarmActor__h

#include <memory>

#include "Bt/AlarmClock/I_Alarm.h"

namespace Bt {
namespace AlarmClock {

class I_AlarmActor {
   public:
      virtual ~I_AlarmActor() {}

      virtual void activateAlarm(std::shared_ptr<I_Alarm> pAlarm) = 0;
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_I_AlarmActor__h

