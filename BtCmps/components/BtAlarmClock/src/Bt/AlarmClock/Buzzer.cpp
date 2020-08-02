//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::Buzzer
//
//*************************************************************************************************

#include "Bt/AlarmClock/Buzzer.h"

#include <Bt/Core/ConstMath.h>

namespace Bt {
namespace AlarmClock {
namespace {
   constexpr uint8_t cDutyResolutionInBits = 16;
   constexpr uint32_t cDuty = Core::ConstMath::pow(2, cDutyResolutionInBits-1); 
}

Buzzer::Buzzer(Peripherals::I_PwmTimer& pPwmTimer, Peripherals::I_PwmOut& pPwmOut)
: mPwmTimer(pPwmTimer), mPwmOut(pPwmOut), mState(false) {

}

Buzzer::~Buzzer() {

}

void Buzzer::state(bool pState) {
   mState = pState;
   mPwmOut.write(pState ? cDuty : 0);
}

void Buzzer::configure(double pFrequency) {
   mPwmTimer.configure(pFrequency);
}

} // namespace AlarmClock
} // namespace Bt

