//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_AlarmActorMock__h
#define INC__Bt_AlarmClock_AlarmActorMock__h

#include <trompeloeil.hpp>

#include "Bt/AlarmClock/I_AlarmActor.h"

namespace Bt {
namespace AlarmClock {

class AlarmActorMock : public I_AlarmActor {
   public:
      MAKE_MOCK1(activateAlarm, void(std::shared_ptr<I_Alarm>),override);
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_AlarmActorMock__h

