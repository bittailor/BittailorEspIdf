//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::Clock
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_Clock__h
#define INC__Bt_AlarmClock_Clock__h

#include <Bt/Core/I_Time.h>
#include <Bt/Core/I_Timezone.h>
#include <Bt/Concurrency/I_SchedulingExecutionContext.h>
#include <Bt/Events/Events.h>

#include "Bt/AlarmClock/I_Clock.h"

namespace Bt {
namespace AlarmClock {

class Clock : public I_Clock
{
   public:
      Clock(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Core::I_Time& pTime, Core::I_Timezone& pTimezone);
      Clock(const Clock&) = delete;
      Clock& operator=(const Clock&) = delete;
      ~Clock();

      Core::I_Timezone::TimeTuple local();

   private:
      void onTimeSyncCompleted();
      void onTick();

      Concurrency::I_SchedulingExecutionContext& mExecutionContext;
      Core::I_Time& mTime; 
      Core::I_Timezone& mTimezone;
      Events::Subscription<Core::I_Time::TimeSyncCompleted> mTimeSyncCompletedSubscription;
      Concurrency::I_SchedulingExecutionContext::PeriodicTimer mTickTimer;
      Core::I_Timezone::TimeTuple mLocal;
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_Clock__h

