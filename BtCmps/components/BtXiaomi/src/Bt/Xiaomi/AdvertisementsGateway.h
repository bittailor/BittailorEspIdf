//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Xiaomi::AdvertisementsGateway
//
//*************************************************************************************************

#ifndef INC__Bt_Xiaomi_AdvertisementsGateway__h
#define INC__Bt_Xiaomi_AdvertisementsGateway__h

#include <Bt/Bluetooth/I_BleAdvertismentScanner.h>
#include <Bt/Protocols/I_MqttController.h>
#include <Bt/Events/Events.h>

namespace Bt {
namespace Xiaomi {

class AdvertisementsGateway : private Bluetooth::I_BleAdvertismentScanner::I_Listener
{
   public:
      AdvertisementsGateway(Concurrency::I_ExecutionContext& pExecutionContext, Bluetooth::I_BleAdvertismentScanner& pScanner, Protocols::I_MqttController& pMqttController);
      AdvertisementsGateway(const AdvertisementsGateway&) = delete;
      AdvertisementsGateway& operator=(const AdvertisementsGateway&) = delete;
      ~AdvertisementsGateway();

   private:
      virtual void onAdvertisment(std::shared_ptr<Bt::Bluetooth::BleDeviceInfo> pDeviceInfo);  

      Bluetooth::I_BleAdvertismentScanner& mScanner;   
      Protocols::I_MqttController& mMqttController;
      Bt::Events::Subscription<Protocols::I_MqttController::Connected> mOnConnected;
      Bt::Events::Subscription<Protocols::I_MqttController::Disconnected> mOnDisconnected;
      bool mMqttConnected;
};

} // namespace Xiaomi
} // namespace Bt

#endif // INC__Bt_Xiaomi_AdvertisementsGateway__h

