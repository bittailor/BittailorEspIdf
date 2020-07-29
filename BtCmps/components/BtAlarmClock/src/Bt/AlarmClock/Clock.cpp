//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::Clock
//
//*************************************************************************************************

#include "Bt/AlarmClock/Clock.h"

#include <iostream>

#include "Bt/AlarmClock/Tag.h"

namespace Bt {
namespace AlarmClock {

Clock::Clock(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Core::I_Time& pTime, Core::I_Timezone& pTimezone)
: mExecutionContext(pExecutionContext), mTime(pTime), mTimezone(pTimezone)
, mTimeSyncCompletedSubscription(pExecutionContext, [this](auto pEvent){onTimeSyncCompleted();}){
}

Clock::~Clock() {
   mTickTimer->cancel();
}

Core::I_Timezone::TimeTuple Clock::local() {
   return mLocal;   
}

void Clock::onTimeSyncCompleted() {
   ESP_LOGI(TAG, "Start tick for Clock");
   mTickTimer = mExecutionContext.callPeriodically(std::chrono::seconds(1), [this](auto& pTimer){
      onTick();
   });
   onTick();
}

void Clock::onTick() {
   auto lastLocal = mLocal;
   mLocal = mTimezone.localTime(mTime.now());

   if(mLocal.time.seconds() != lastLocal.time.seconds()) {
      Events::publish(I_Clock::SecondUpdate{});
   }
   if(mLocal.time.minutes() != lastLocal.time.minutes()) {
      Events::publish(I_Clock::MinuteUpdate{});
   }
   if(mLocal.time.hours() != lastLocal.time.hours()) {
      Events::publish(I_Clock::HourUpdate{});
   }

}



} // namespace AlarmClock
} // namespace Bt

