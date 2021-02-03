//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Xiaomi::AdvertisementsGateway
//
//*************************************************************************************************

#ifndef INC__Bt_Xiaomi_AdvertisementsGateway__h
#define INC__Bt_Xiaomi_AdvertisementsGateway__h

#include <Bt/Bluetooth/I_BleController.h>

namespace Bt {
namespace Xiaomi {

class AdvertisementsGateway
{
   public:
      AdvertisementsGateway(Bluetooth::I_BleController& pBleController);
      AdvertisementsGateway(const AdvertisementsGateway&) = delete;
      AdvertisementsGateway& operator=(const AdvertisementsGateway&) = delete;
      ~AdvertisementsGateway();

   private:
      
};

} // namespace Xiaomi
} // namespace Bt

#endif // INC__Bt_Xiaomi_AdvertisementsGateway__h

