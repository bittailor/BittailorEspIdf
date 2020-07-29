//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::DisplayBacklight
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_DisplayBacklight__h
#define INC__Bt_AlarmClock_DisplayBacklight__h

#include <Bt/Concurrency/I_SchedulingExecutionContext.h>
#include <Bt/Peripherals/I_RgbLed.h>


namespace Bt {
namespace AlarmClock {

class DisplayBacklight
{
   public:
      DisplayBacklight(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Peripherals::I_RgbLed& pRgbLed);
      DisplayBacklight(const DisplayBacklight&) = delete;
      DisplayBacklight& operator=(const DisplayBacklight&) = delete;
      ~DisplayBacklight();

      void configure(uint8_t pHue, uint8_t pBrightness, const std::chrono::milliseconds& pAutoOffTime);

      void state(bool pOn);
      void on() {state(true);}
      void off() {state(false);}

   private:
      Concurrency::I_SchedulingExecutionContext& mExecutionContext;
      Peripherals::I_RgbLed& mRgbLed;
      uint8_t mHue;
      uint8_t mBrightness;
      std::chrono::milliseconds mAutoOffTime;
      Concurrency::I_SchedulingExecutionContext::OnceTimer mAutoOffTimer;
      bool mState;
      
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_DisplayBacklight__h

