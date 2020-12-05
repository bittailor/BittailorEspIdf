//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Xiaomi::Gateway
//
//*************************************************************************************************

#include "Bt/Xiaomi/Gateway.h"

#include <Bt/Core/Logging.h>

#include "Bt/Devices/Xiaomi/HumiditySensor.h"

namespace Bt {
namespace Xiaomi {
    using namespace Bt::Devices::Xiaomi;

namespace {
    constexpr const char* TAG = "Bt::Xiaomi::Gateway";
    Bluetooth::BleAddress cAddress = Bluetooth::BleAddress::from48BitBe(Bluetooth::BleAddress::Address48Bit{{0xA4,0xC1,0x38,0x1D,0x2D,0x67}});
}

Gateway::Gateway(Concurrency::I_ExecutionContext& pExecutionContext, Bluetooth::I_BleController& pBleController)
: mExecutionContext(pExecutionContext)
, mBleController(pBleController)
, mOnBleSynced(mExecutionContext,[this](auto pEvent){onBleSynced();})
, mOnMqttConnected(mExecutionContext,[this](auto pEvent){onMqttConnected();}) 
, mBleSynced(false), mMqttSynced(false) {

}

Gateway::~Gateway() {
}

void Gateway::onBleSynced() {
    mBleSynced = true;
    auto device = std::make_shared<Bt::Devices::Xiaomi::HumiditySensor>(mExecutionContext, mBleController, cAddress);
    mDevices.insert({cAddress,device});
    onEvent();
}

void Gateway::onMqttConnected() {
    mMqttSynced = true;
    onEvent();
}

void Gateway::onEvent() {
    if(!(mBleSynced && mMqttSynced)) {
        return;
    }
    ESP_LOGI(TAG, "lets connect to %d devices", mDevices.size());
    for (auto &&entry : mDevices)
    {
        entry.second->connect();
    }
    

}

} // namespace Xiaomi
} // namespace Bt

