//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::AlarmActor
//
//*************************************************************************************************

#include "Bt/AlarmClock/AlarmActor.h"

#include "Bt/AlarmClock/Tag.h"



namespace Bt {
namespace AlarmClock {

AlarmActor::AlarmActor(Concurrency::I_SchedulingExecutionContext& pExecutionContext, I_Buzzer& pBuzzer)
: mExecutionContext(pExecutionContext), mBuzzer(pBuzzer) {

}

AlarmActor::~AlarmActor() {

}

void AlarmActor::activateAlarm(std::shared_ptr<I_Alarm> pAlarm) {
   ESP_LOGI(TAG, "activate alarm");
   mBuzzer.on();
   mTimeUpTimer = mExecutionContext.callOnce(std::chrono::seconds(30),[this](){
      ESP_LOGI(TAG, "time up => alarm off");
      mBuzzer.off();   
   });
}

void AlarmActor::stoppPressed() {
   
   if(mTimeUpTimer) {
      ESP_LOGI(TAG, "stopp pressed => alarm off");
      mBuzzer.off();
      mTimeUpTimer->cancel();
      mTimeUpTimer.reset();
   } else {
      ESP_LOGI(TAG, "stopp pressed => but no alarm running");   
   }
}

} // namespace AlarmClock
} // namespace Bt

