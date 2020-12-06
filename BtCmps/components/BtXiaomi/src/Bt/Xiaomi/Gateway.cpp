//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Xiaomi::Gateway
//
//*************************************************************************************************

#include "Bt/Xiaomi/Gateway.h"

#include <ArduinoJson.h>

#include <Bt/Core/Logging.h>
#include <Bt/Core/StringBuilder.h>

#include "Bt/Devices/Xiaomi/HumiditySensor.h"

namespace Bt {
namespace Xiaomi {
    using namespace Bt::Devices::Xiaomi;

namespace {
    constexpr const char* TAG = "Bt::Xiaomi::Gateway";
    Bluetooth::BleAddress cAddresses[] = {
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0x1D,0x2D,0x67}}),
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0xEF,0x61,0x6E}}),
    };
}

Gateway::Gateway(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Bluetooth::I_BleController& pBleController, Protocols::I_MqttController& pMqtt)
: mExecutionContext(pExecutionContext)
, mBleController(pBleController)
, mMqtt(pMqtt)
, mOnBleSynced(mExecutionContext,[this](auto pEvent){onBleSynced();})
, mOnMqttConnected(mExecutionContext,[this](auto pEvent){onMqttConnected();}) 
, mBleSynced(false), mMqttSynced(false) {
}

Gateway::~Gateway() {
}

void Gateway::onBleSynced() {
    mBleSynced = true;
    for (auto&& address : cAddresses)
    {
        auto device = std::make_shared<Bt::Devices::Xiaomi::HumiditySensor>(mExecutionContext, mBleController, address);
        mDevices.insert({address,device});    
    } 
    onEvent();
}

void Gateway::onMqttConnected() {
    mMqttSynced = true;
    mExecutionContext.callPeriodically(std::chrono::seconds(10),[this](auto&& pTimer){

    });
    onEvent();
}

void Gateway::onEvent() {
    if(!(mBleSynced && mMqttSynced)) {
        return;
    }
    ESP_LOGI(TAG, "lets connect to %d devices", mDevices.size());
    for (auto &&entry : mDevices)
    {
        entry.second->connect([this](const std::string& pId, const I_Device::Values& pValues){
            onReading(pId,pValues);
        });
    }
}

void Gateway::onReading(const std::string& pId, const I_Device::Values& pValues) {
    DynamicJsonDocument json(1024);
    for (auto&& value : pValues)
    {
        json[value.first].set(value.second); 
    }   
    Core::DefaultStringBuilder topic;
    topic.append("bittailor/home/sensor/xiaomi/%s/reading",pId.c_str()); 
    mMqtt.publish(topic.c_str(), json.as<std::string>());
}

} // namespace Xiaomi
} // namespace Bt

