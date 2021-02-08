//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleAdvertismentScanner
//
//*************************************************************************************************

#ifndef INC__Bt_Bluetooth_BleAdvertismentScanner__h
#define INC__Bt_Bluetooth_BleAdvertismentScanner__h

#include <Bt/Bluetooth/I_BleAdvertismentScanner.h>
#include <Bt/Bluetooth/I_BleController.h>
#include <Bt/Events/Events.h>
#include <Bt/Concurrency/I_SchedulingExecutionContext.h>


namespace Bt {
namespace Bluetooth {

class BleAdvertismentScanner : public I_BleAdvertismentScanner 
{
   public:
      BleAdvertismentScanner(Concurrency::I_SchedulingExecutionContext& pExecutionContext, I_BleController& pController);
      BleAdvertismentScanner(const BleAdvertismentScanner&) = delete;
      BleAdvertismentScanner& operator=(const BleAdvertismentScanner&) = delete;
      ~BleAdvertismentScanner();

      virtual void add(I_Listener& pListener);
      virtual void remove(I_Listener& pListener);

   private:
      template<typename E> using Subscription = Bt::Events::Subscription<E>;

      void onBleSynced(I_BleController::Synced pEvent);
      void onBleReset(I_BleController::Reset pEvent);
      void onAdvertisment(std::shared_ptr<Bt::Bluetooth::BleDeviceInfo> pDeviceInfo);

      Concurrency::I_SchedulingExecutionContext& mExecutionContext;
      I_BleController& mController;
      Subscription<I_BleController::Synced> mOnBleSynced;
      Subscription<I_BleController::Reset> mOnBleReset;
      std::mutex mMutex;
      std::list<I_Listener*> mListeners;
      std::shared_ptr<I_BleDeviceDiscoveryAgent> mDiscoveryAgent;


      
};

} // namespace Bluetooth
} // namespace Bt

#endif // INC__Bt_Bluetooth_BleAdvertismentScanner__h

