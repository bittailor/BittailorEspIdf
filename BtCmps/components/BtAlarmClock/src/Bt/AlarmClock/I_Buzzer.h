//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::AlarmClock::I_Buzzer
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_I_Buzzer__h
#define INC__Bt_AlarmClock_I_Buzzer__h

namespace Bt {
namespace AlarmClock {

class I_Buzzer {
   public:
      virtual ~I_Buzzer() {}

      virtual void configure(double pFrequency)  = 0;
    
      virtual void state(bool pState)   = 0;
      virtual bool state()const  = 0;

      void on() {state(true);}
      void off() {state(false);}

};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_I_Buzzer__h

