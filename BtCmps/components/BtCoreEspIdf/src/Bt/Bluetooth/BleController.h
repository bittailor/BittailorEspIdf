//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleController
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleController__h
#define INC__Bt_Bluetooth_BleController__h

#include <queue>

#include <Bt/Bluetooth/I_BleController.h>
#include <Bt/Concurrency/I_ExecutionContext.h>

namespace Bt {
namespace Bluetooth {

class BleController : public I_BleController
{
   public:
      BleController(Concurrency::I_ExecutionContext& pExecutionContext);
      BleController(const BleController&) = delete;
      BleController& operator=(const BleController&) = delete;
      ~BleController();

      virtual std::shared_ptr<I_BleDeviceDiscoveryAgent>  createDeviceDiscoveryAgent(I_BleDeviceDiscoveryAgent::OnDiscover pOnDiscover = nullptr,I_BleDeviceDiscoveryAgent::OnDiscoverComplete pOnDiscoverComplete = nullptr);
      virtual std::shared_ptr<I_BleClient>  createClient(I_BleClient::I_Listener& pListener);

      void enqueConnect(std::function<void()> pConnect);
      void dequeConnect();
      
   private:
      static void onHostResetStatic(int pReason);
      static void onHostAndControllerSyncedStatic(); 

      void onHostReset(int pReason);
      void onHostAndControllerSynced(); 

      Concurrency::I_ExecutionContext& mExecutionContext;
      std::queue<std::function<void()>> mConnectQueue;
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleController__h

