//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::System::Vitals
//
//*************************************************************************************************

#ifndef INC__Bt_System_Vitals__h
#define INC__Bt_System_Vitals__h

#include <string>

#include <Bt/Concurrency/I_SchedulingExecutionContext.h>
#include <Bt/Events/Events.h>
#include <Bt/Protocols/I_MqttController.h>

namespace Bt {
namespace System {

class Vitals
{
   public:
      Vitals(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Protocols::I_MqttController& pMqtt);
      Vitals(const Vitals&) = delete;
      Vitals& operator=(const Vitals&) = delete;
      ~Vitals();

      std::string vitalsJson();
   
   private:
      void onMqttConnected();
      void publishVitals();


      Concurrency::I_SchedulingExecutionContext& mExecutionContext;
      Protocols::I_MqttController& mMqtt;
      Events::Subscription<Protocols::I_MqttController::Connected> mOnMqttConnected;
      std::string mChipId;
      std::string mTopic;
       
};

} // namespace System
} // namespace Bt

#endif // INC__Bt_System_Vitals__h

