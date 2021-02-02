//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::System::OtaUpdate
//
//*************************************************************************************************

#ifndef INC__Bt_System_OtaUpdate__h
#define INC__Bt_System_OtaUpdate__h

#include <esp_ota_ops.h>

#include <Bt/Concurrency/I_SchedulingExecutionContext.h>
#include <Bt/Protocols/MqttSubscription.h>
#include <Bt/Protocols/I_MqttController.h>

namespace Bt {
namespace System {

class OtaUpdate
{
   public:
      OtaUpdate(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Protocols::I_MqttController& pMqttController);
      OtaUpdate(const OtaUpdate&) = delete;
      OtaUpdate& operator=(const OtaUpdate&) = delete;
      ~OtaUpdate();


   private:
      void onMessage(const Protocols::RawMqttMessage& pMessage);
      void onRestartMessage(std::shared_ptr<Protocols::MqttMessage> pMessage);
      
      
      Concurrency::I_SchedulingExecutionContext& mExecutionContext; 
      Protocols::I_MqttController& mMqttController;
      Protocols::MqttRawSubscription mUpdateSubscription;
      Protocols::MqttMessageSubscription mRebootSubscription;
      esp_ota_handle_t mUpdateHandle;
      const esp_partition_t* mUpdatePartition;
      
};

} // namespace System
} // namespace Bt

#endif // INC__Bt_System_OtaUpdate__h

