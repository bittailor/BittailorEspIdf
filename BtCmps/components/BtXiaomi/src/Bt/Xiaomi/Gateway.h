//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Xiaomi::Gateway
//
//*************************************************************************************************

#ifndef INC__Bt_Xiaomi_Gateway__h
#define INC__Bt_Xiaomi_Gateway__h

#include <map>
#include <memory>

#include <Bt/Bluetooth/I_BleController.h>
#include <Bt/Events/Events.h>
#include <Bt/Protocols/I_MqttController.h>
#include <Bt/Devices/Xiaomi/I_Device.h>

namespace Bt {
namespace Xiaomi {

class Gateway
{
   public:
      using Devices = std::map<Bt::Bluetooth::BleAddress, std::shared_ptr<Bt::Devices::Xiaomi::I_Device>>; 

      template<typename E> using EventSubscription = Bt::Events::Subscription<E>;

      Gateway(Concurrency::I_ExecutionContext& pExecutionContext, Bluetooth::I_BleController& pBleController);
      Gateway(const Gateway&) = delete;
      Gateway& operator=(const Gateway&) = delete;
      ~Gateway();

   private:
      void onBleSynced();
      void onMqttConnected();
      void onEvent();

      Concurrency::I_ExecutionContext& mExecutionContext;
      Bluetooth::I_BleController& mBleController;
      EventSubscription<Bluetooth::I_BleController::Synced> mOnBleSynced;
      EventSubscription<Protocols::I_MqttController::Connected> mOnMqttConnected;
      Devices mDevices;
      bool mBleSynced;
      bool mMqttSynced;

      
};

} // namespace Xiaomi
} // namespace Bt

#endif // INC__Bt_Xiaomi_Gateway__h

