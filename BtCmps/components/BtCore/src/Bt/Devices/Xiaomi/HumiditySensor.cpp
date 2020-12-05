//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Devices::Xiaomi::HumiditySensor
//
//*************************************************************************************************

#include "Bt/Devices/Xiaomi/HumiditySensor.h"

#include <Bt/Core/Logging.h>
#include <Bt/Bluetooth/BleUuid.h>

namespace Bt {
namespace Devices {
namespace Xiaomi {
namespace {
    constexpr const char* TAG = "Bt::Devices::Xiaomi::HumiditySensor";

    constexpr Bluetooth::BleUuid cServiceUuid =  
        Bluetooth::BleUuid::from128BitBE(
            Bluetooth::BleUuid::Uuid128Bit{
                0xeb,0xe0,0xcc,0xb0,0x7a,0x0a,0x4b,0x0c,0x8a,0x1a,0x6f,0xf2,0x99,0x7d,0xa3,0xa6
            }); // ebe0ccb0-7a0a-4b0c-8a1a-6ff2997da3a6

    constexpr Bluetooth::BleUuid sTemperatureAndHumidityCharacteristicUUID =  
        Bluetooth::BleUuid::from128BitBE(
            Bluetooth::BleUuid::Uuid128Bit{
                0xeb,0xe0,0xcc,0xc1,0x7a,0x0a,0x4b,0x0c,0x8a,0x1a,0x6f,0xf2,0x99,0x7d,0xa3,0xa6
            }); // ebe0ccc1-7a0a-4b0c-8a1a-6ff2997da3a6

    constexpr Bluetooth::BleUuid sSetConnectionIntervalCharacteristicUUID =  
        Bluetooth::BleUuid::from128BitBE(
            Bluetooth::BleUuid::Uuid128Bit{
                0xeb,0xe0,0xcc,0xd8,0x7a,0x0a,0x4b,0x0c,0x8a,0x1a,0x6f,0xf2,0x99,0x7d,0xa3,0xa6
            }); // ebe0ccd8-7a0a-4b0c-8a1a-6ff2997da3a6

}

void HumiditySensor::registerAtFactory(Concurrency::I_ExecutionContext& pExecutionContext, DeviceFactory& pFactory, Bt::Bluetooth::I_BleController& pBleController) {
    pFactory.registerDevice(HumiditySensor::cDeviceTypeId,[&pExecutionContext,&pBleController](auto&& pDeviceInfo){
        return std::make_shared<HumiditySensor>(pExecutionContext, pBleController, pDeviceInfo);
    });   
}



HumiditySensor::HumiditySensor(Concurrency::I_ExecutionContext& pExecutionContext, Bt::Bluetooth::I_BleController& pBleController, const DeviceInfo& pDeviceInfo)
: HumiditySensor(pExecutionContext, pBleController, pDeviceInfo.bleDeviceInfo()->address()) {
}

HumiditySensor::HumiditySensor(Concurrency::I_ExecutionContext& pExecutionContext, Bt::Bluetooth::I_BleController& pBleController, const Bluetooth::BleAddress& pBleAddress)
: mExecutionContext(pExecutionContext), mBleClient(pBleController.createClient(*this)), mAddress(pBleAddress) {
}

HumiditySensor::~HumiditySensor() {
}

bool HumiditySensor::connect() {
    ESP_LOGI(TAG, "connect to => %s", mAddress.toString().c_str()); 
    return mBleClient->connect(mAddress);
}

void HumiditySensor::onConnect() {
    ESP_LOGI(TAG, "[%s]  onConnect", mAddress.toString().c_str()); 
    mExecutionContext.call([this](){
        mBleClient->getService(cServiceUuid);    
    });
}

void HumiditySensor::onDisconnect() {
    ESP_LOGI(TAG, "[%s] onDisconnect", mAddress.toString().c_str()); 
}

void HumiditySensor::onServiceDiscover(BleService pService) {
    ESP_LOGI(TAG, "[%s] onServiceDiscover : %s", mAddress.toString().c_str(), pService->toString().c_str());
    mService = pService;
    mExecutionContext.call([this](){
        mService->getCharacteristic(sTemperatureAndHumidityCharacteristicUUID); 
    });    
}

void HumiditySensor::onCharacteristicDiscover(BleCharacteristic pCharacteristic) {
    ESP_LOGI(TAG, "[%s] onCharacteristicDiscover : %s", mAddress.toString().c_str(), pCharacteristic->toString().c_str());
    mTemperatureAndHumidityCharacteristic = pCharacteristic;  
    mExecutionContext.call([this](){
        mTemperatureAndHumidityCharacteristic->subscribe([](){
            
        }); 
    });  
}

} // namespace Xiaomi
} // namespace Devices
} // namespace Bt
