//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::AlarmClock::I_Alarm
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_I_Alarm__h
#define INC__Bt_AlarmClock_I_Alarm__h

namespace Bt {
namespace AlarmClock {

class I_Alarm {
   public:
      static const uint32_t cMaxDelayInMinutesToStillTriggerAlarm = 30;

      struct Check {bool mTrigger; bool mKeep;};

      virtual ~I_Alarm() {}

      virtual Check check() = 0;
      virtual std::chrono::system_clock::time_point nextInLocal() = 0;
      virtual std::chrono::seconds seconds() = 0;

};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_I_Alarm__h

