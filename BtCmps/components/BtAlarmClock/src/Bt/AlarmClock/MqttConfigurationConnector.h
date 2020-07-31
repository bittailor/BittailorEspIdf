//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::AlarmClock::MqttConfigurationConnector
//
//*************************************************************************************************

#ifndef INC__Bt_AlarmClock_MqttConfigurationConnector__h
#define INC__Bt_AlarmClock_MqttConfigurationConnector__h

#include <Bt/Protocols/MqttSubscription.h>
#include <Bt/Protocols/I_MqttController.h>

#include "Bt/AlarmClock/ConfigurationController.h"

namespace Bt {
namespace AlarmClock {

class MqttConfigurationConnector
{
   public:
      MqttConfigurationConnector(Concurrency::I_ExecutionContext& pExecutionContext, ConfigurationController& pConfigurationController, Bt::Protocols::I_MqttController& pMqttController);
      MqttConfigurationConnector(const MqttConfigurationConnector&) = delete;
      MqttConfigurationConnector& operator=(const MqttConfigurationConnector&) = delete;
      ~MqttConfigurationConnector();

   private:
      void onMqttGetMessage(std::shared_ptr<Bt::Protocols::MqttMessage> pMessage);
      void onMqttSetMessage(std::shared_ptr<Bt::Protocols::MqttMessage> pMessage);

      Concurrency::I_ExecutionContext& mExecutionContext;
      ConfigurationController& mConfigurationController;
      Bt::Protocols::I_MqttController& mMqttController;
      Bt::Protocols::MqttSubscription mMqttGetSubscription;
      Bt::Protocols::MqttSubscription mMqttSetSubscription;
      
};

} // namespace AlarmClock
} // namespace Bt

#endif // INC__Bt_AlarmClock_MqttConfigurationConnector__h
