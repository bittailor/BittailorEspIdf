//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::MijiaBle
//
//*************************************************************************************************

#include "Bt/Devices/MijiaBle.h"

namespace Bt {
namespace Devices {
namespace {
    // NimBLEUUID sAdvServiceUUID("fe95");

    // std::string sServiceUUID("ebe0ccb0-7a0a-4b0c-8a1a-6ff2997da3a6");
    // std::string sCharacteristicUUID("ebe0ccc1-7a0a-4b0c-8a1a-6ff2997da3a6");
    // std::string sCharacteristicUUIDConnectionInterval("ebe0ccd8-7a0a-4b0c-8a1a-6ff2997da3a6");

    std::function<void(std::vector<std::string> sensorAddresses)> sDevicesScanCompletedCallback;

    // void onScanComplete(NimBLEScanResults pResult) {
    //     std::vector<std::string> sensorAddresses;
    //     log_i("... onScanComplete found %d devices filter Devices ...\n", pResult.getCount());
    //     for (NimBLEAdvertisedDevice* device : pResult)
    //     {
    //         log_i(" - %s haveServiceData=%s", device->getAddress().toString().c_str(),device->haveServiceData()? "true":"false");
    //         if(device->haveServiceData()) {
    //             log_i("   -- getServiceDataUUID = %s", device->getServiceDataUUID().toString().c_str());
    //             if(device->getServiceDataUUID().equals(sAdvServiceUUID)) {
    //                 log_i("   -+- push back %s", device->getAddress().toString().c_str());
    //                 sensorAddresses.push_back(device->getAddress().toString());
    //             } else {
    //                 log_i("   -x- %s != %s", device->getServiceDataUUID().toString().c_str(),sAdvServiceUUID.toString().c_str());    
    //             }
    //         }        
    //     }
    //     Serial.printf("... found %d Devices\n", sensorAddresses.size());
    //     if(sDevicesScanCompletedCallback) {
    //         sDevicesScanCompletedCallback(std::move(sensorAddresses));
    //     } else {
    //         log_e("sDevicesScanCompletedCallback is nullptr");     
    //     } 
    // }
}




bool MijiaBle::scanDevicesAddresses(uint32_t pDuration, std::function<void(std::vector<Bluetooth::BleAddress> sensorAddresses)> pDevicesScanCompletedCallback) {
//   std::vector<std::string> Devices;
//   NimBLEScan& scanner = *NimBLEDevice::getScan();
//   scanner.setActiveScan(true);
//   scanner.setInterval(45);
//   scanner.setWindow(15);

//   log_i("start scanning for %d seconds ...\n", pDuration);
//   sDevicesScanCompletedCallback = pDevicesScanCompletedCallback;
//   if(!scanner.start(pDuration, onScanComplete)) {
//       log_e("scanner.start failed");
//       sDevicesScanCompletedCallback = nullptr;
//       return false;
//   }
  return true;
}

MijiaBle::MijiaBle(std::string pAddress, OnNotification pOnNotification) : mAddress(pAddress), mOnNotification(pOnNotification) /*, mClient(NimBLEDevice::createClient())*/ {
    /*
    log_i("MijiaBle [%s] MijiaBle()", mAddress.c_str());
    mClient->setClientCallbacks(this);
    if(!mClient->connect(pAddress)) {
        log_e("MijiaBle [%s] connect failed", mAddress.c_str());
        return;
    }
    log_i("MijiaBle [%s] connectd", mAddress.c_str());

    NimBLERemoteService* pRemoteService = mClient->getService(sServiceUUID);
    if (pRemoteService == nullptr) {
        log_e("MijiaBle [%s] failed to find our service UUID: %s", mAddress.c_str(), sServiceUUID.c_str());
        mClient->disconnect();
        return;
    }
    log_i("MijiaBle [%s]found our service", mAddress.c_str());

    NimBLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(sCharacteristicUUID);
    if (pRemoteCharacteristic == nullptr) {
        log_e("MijiaBle [%s] failed to get characteristic UUID: %s", mAddress.c_str(), sCharacteristicUUID.c_str());      
        mClient->disconnect();
    }
    log_i("MijiaBle [%s] found our characteristic", mAddress.c_str());
    pRemoteCharacteristic->subscribe(true,[this](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify){
        this->onNotification(pBLERemoteCharacteristic, pData, length, isNotify);
    },true);

    NimBLERemoteCharacteristic* pConnectionInterval = pRemoteService->getCharacteristic(sCharacteristicUUIDConnectionInterval);
    if (pConnectionInterval == nullptr) {
        log_e("MijiaBle [%s] failed to get Connection Interval characteristic UUID: %s", mAddress.c_str(), sCharacteristicUUIDConnectionInterval.c_str());      
        mClient->disconnect();
    }
    log_i("MijiaBle [%s] found Connection Interval characteristic", mAddress.c_str());
    //std::array<uint8_t,3> connectionInterval{0xf4,0x01, 0x00};
    std::array<uint8_t,3> connectionInterval{0xD0,0x07, 0x00};
    pConnectionInterval->writeValue(connectionInterval.data(), connectionInterval.size(),false);
    */
}

MijiaBle::~MijiaBle() {
    /*
    if(mClient->isConnected()) {
        mClient->disconnect();
    }
    NimBLEDevice::deleteClient(mClient);
    */
}

// int MijiaBle::rssi() {
//     return mClient->getRssi();   
// }

// void MijiaBle::onConnect(NimBLEClient* pClient) {
//     log_i("MijiaBle [%s] onConnect", mAddress.c_str());
// }

// void MijiaBle::onDisconnect(NimBLEClient* pClient) {
//     log_i("MijiaBle [%s] onDisconnect", mAddress.c_str());
// }

// void MijiaBle::onNotification(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
//     if(length < 5) {
//         log_e("MijiaBle [%s] onNotification data length = < 5!", mAddress.c_str(), length);
//         return;
//     }
//     float temperature = (pData[0] | (pData[1] << 8)) * 0.01; //little endian 
//     float humidity = pData[2];
//     float battery = (pData[3] | (pData[4] << 8)) * 0.001;
//     if(mOnNotification) {
//         mOnNotification(temperature, humidity, battery);    
//     }
//     log_d("MijiaBle [%s] temperature = %.1f : humidity = %.1f : battery = %.1f\n", mAddress.c_str(), temperature, humidity, battery);
// }


} // namespace Devices
} // namespace Bt

