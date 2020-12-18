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
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0x1D,0x2D,0x67}}), // corridor
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0xEF,0x61,0x6E}}), // balcony
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0x7B,0x1D,0xDB}}), // kitchen
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0xF7,0xA8,0x55}}), // bedroom f&b
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0xF5,0x4C,0x21}}), // bathroom¨
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0x86,0xE1,0xD1}}), // hanna
        Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0x31,0xE1,0x15}}), // jonathan
        //Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0x90,0xEC,0x72}}), // living room¨
    };
}

Gateway::Gateway(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Bluetooth::I_BleController& pBleController, Protocols::I_MqttController& pMqtt)
: mExecutionContext(pExecutionContext)
, mBleController(pBleController)
, mMqtt(pMqtt)
, mOnBleSynced(mExecutionContext,[this](auto pEvent){onBleSynced();})
, mOnMqttConnected(mExecutionContext,[this](auto pEvent){onMqttConnected();}) 
, mMqttConnected(false) {
}

Gateway::~Gateway() {
}

void Gateway::onBleSynced() {
    for (auto&& address : cAddresses)
    {
        auto device = std::make_shared<Bt::Devices::Xiaomi::HumiditySensor>(mExecutionContext, mBleController, address);
        mDevices.insert({address,device});    
    } 
    for (auto &&entry : mDevices)
    {
        entry.second->connect([this](const std::string& pId, const I_Device::Values& pValues){
            onReading(pId,pValues);
        });
    }
}

void Gateway::onMqttConnected() {
    mMqttConnected = true;
}

void Gateway::onReading(const std::string& pId, const I_Device::Values& pValues) {
    if(!mMqttConnected) {
        return;
    }
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

