//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::AlarmActor
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_AlarmActor__h
#define INC__Bt_AlarmClock_AlarmActor__h

#include <Bt/Concurrency/I_SchedulingExecutionContext.h>

#include "Bt/AlarmClock/I_Buzzer.h"
#include "Bt/AlarmClock/I_AlarmActor.h"

namespace Bt {
namespace AlarmClock {

class AlarmActor : public I_AlarmActor
{
   public:
      AlarmActor(Concurrency::I_SchedulingExecutionContext& pExecutionContext, I_Buzzer& pBuzzer);
      AlarmActor(const AlarmActor&) = delete;
      AlarmActor& operator=(const AlarmActor&) = delete;
      ~AlarmActor();

      virtual void activateAlarm(std::shared_ptr<I_Alarm> pAlarm) override;
      void stoppPressed();

   private:
      Concurrency::I_SchedulingExecutionContext& mExecutionContext;
      I_Buzzer& mBuzzer;
      Concurrency::I_SchedulingExecutionContext::OnceTimer mTimeUpTimer;
      
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_AlarmActor__h

