//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::MijiaBle
//
//*************************************************************************************************

#ifndef INC__Bt_Devices_MijiaBle__h
#define INC__Bt_Devices_MijiaBle__h

#include <functional>

#include <Bt/Bluetooth/BleDeviceInfo.h>

namespace Bt {
namespace Devices {

class MijiaBle //: private NimBLEClientCallbacks 
{
   public:
      static bool scanDevicesAddresses(uint32_t pDuration, std::function<void(std::vector<Bluetooth::BleAddress> sensorAddresses)> pDevicesScanCompletedCallback);
      
      typedef std::function<void(float pTemperature, float pHumidity, float pBattery)> OnNotification;

      MijiaBle(std::string pAddress, OnNotification pOnNotification);
      MijiaBle(const MijiaBle&) = delete;
      MijiaBle& operator=(const MijiaBle&) = delete;
      ~MijiaBle();

      // const std::string& address() {return mAddress;}
      // int rssi();

   private:
   
      // virtual void onConnect(NimBLEClient* pClient);
      // virtual void onDisconnect(NimBLEClient* pClient); 
      //virtual bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params);
      //virtual uint32_t onPassKeyRequest();
      //virtual void onAuthenticationComplete(ble_gap_conn_desc* desc);
      //virtual bool onConfirmPIN(uint32_t pin);

      //void onNotification(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);

      std::string mAddress;
      OnNotification mOnNotification;
      //NimBLEClient* mClient;

};

} // namespace Devices
} // namespace Bt

#endif // INC__Bt_Devices_MijiaBle__h

