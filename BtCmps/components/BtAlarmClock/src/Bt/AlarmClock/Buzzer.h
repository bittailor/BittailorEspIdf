//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::Buzzer
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_Buzzer__h
#define INC__Bt_AlarmClock_Buzzer__h

#include <Bt/Peripherals/I_PwmTimer.h>
#include <Bt/Peripherals/I_PwmOut.h>

#include "Bt/AlarmClock/I_Buzzer.h"

namespace Bt {
namespace AlarmClock {

class Buzzer : public I_Buzzer
{
   public:
      Buzzer(Peripherals::I_PwmTimer& pPwmTimer, Peripherals::I_PwmOut& pPwmOut);
      Buzzer(const Buzzer&) = delete;
      Buzzer& operator=(const Buzzer&) = delete;
      virtual ~Buzzer();

      virtual void configure(double pFrequency);
      virtual void state(bool pState);
      virtual bool state()const {return mState;}

   private:
      Peripherals::I_PwmTimer& mPwmTimer;
      Peripherals::I_PwmOut& mPwmOut;
      bool mState;
      
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_Buzzer__h

