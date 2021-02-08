//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Xiaomi::AdvertisementsGateway
//
//*************************************************************************************************

#include "Bt/Xiaomi/AdvertisementsGateway.h"

#include <Bt/Core/Logging.h>
#include <Bt/Core/StringUtilities.h>

namespace Bt {
namespace Xiaomi {
namespace {
    constexpr const char* TAG = "Bt::Xiaomi::AdvertisementsGateway";

    constexpr Bt::Bluetooth::BleUuid cServiceUuid =  
            Bt::Bluetooth::BleUuid::from128BitBE(
               Bt::Bluetooth::BleUuid::Uuid128Bit{
                0x00,0x00,0x18,0x1a, 0x00,0x00, 0x10, 0x00, 0x80,0x00, 0x00,0x80,0x5f,0x9b,0x34,0xfb
            });

}

AdvertisementsGateway::AdvertisementsGateway(Concurrency::I_ExecutionContext& pExecutionContext, Bluetooth::I_BleAdvertismentScanner& pScanner, Protocols::I_MqttController& pMqttController)
: mScanner(pScanner), mMqttController(pMqttController)
, mOnConnected(pExecutionContext, [this](auto pEvent){mMqttConnected=true;}) 
, mOnDisconnected(pExecutionContext, [this](auto pEvent){mMqttConnected=false;})
, mMqttConnected(false) {
    mScanner.add(*this);
}

AdvertisementsGateway::~AdvertisementsGateway() {
    mScanner.remove(*this);
}

void AdvertisementsGateway::onAdvertisment(std::shared_ptr<Bt::Bluetooth::BleDeviceInfo> pDeviceInfo)
{
    if (pDeviceInfo->serviceUuid() == cServiceUuid)
    {
        if (pDeviceInfo->serviceData().size() == 15)
        {
            float temperature = ((pDeviceInfo->serviceData()[7] << 8) | (pDeviceInfo->serviceData()[6])) * 0.01;
            float humidity = ((pDeviceInfo->serviceData()[9] << 8) | (pDeviceInfo->serviceData()[8])) * 0.01;
            float vbattery = ((pDeviceInfo->serviceData()[11] << 8) | (pDeviceInfo->serviceData()[10])) * 0.001;
            uint8_t battery = pDeviceInfo->serviceData()[12];
            uint8_t cnt = pDeviceInfo->serviceData()[13];


            std::string msg = Bt::Core::stringPrintf(
                R"JSON({"rssi":%d,"temperature":%.2f,"humidity":%.2f,"battery":%.3f,"batteryState":%d,"cnt":%d})JSON",
                (int32_t)pDeviceInfo->rssi(),
                temperature, humidity, vbattery,
                battery, (uint32_t)cnt);

            ESP_LOGI(TAG, "[%s] %s", pDeviceInfo->address().toString().c_str(), msg.c_str());

            if(mMqttConnected) {
                mMqttController.publish(
                // bittailor/develop/tests/adv/%s/reading
                // bittailor/home/sensor/xiaomi/%s/reading
                Bt::Core::stringPrintf("bittailor/home/sensor/xiaomi/%s/reading", pDeviceInfo->address().toString().c_str()).c_str(),
                msg);
            } else {
                ESP_LOGW(TAG, "mqtt not connected => drop reading");    
            }
        }
        else
        {
            ESP_LOGW(TAG, "Wrong length %d != 15 for %s", pDeviceInfo->serviceData().size(), pDeviceInfo->serviceUuid().toString().c_str());
        }
    }
}

} // namespace Xiaomi
} // namespace Bt

