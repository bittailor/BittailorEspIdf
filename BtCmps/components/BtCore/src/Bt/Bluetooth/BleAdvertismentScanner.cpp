//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleAdvertismentScanner
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleAdvertismentScanner.h"

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleAdvertismentScanner";
}

BleAdvertismentScanner::BleAdvertismentScanner(Concurrency::I_SchedulingExecutionContext& pExecutionContext, I_BleController& pController)
: mExecutionContext(pExecutionContext)
, mController(pController) 
, mOnBleSynced(mExecutionContext,[this](auto pEvent){onBleSynced(pEvent);})
, mOnBleReset(mExecutionContext,[this](auto pEvent){onBleReset(pEvent);}) {
}

BleAdvertismentScanner::~BleAdvertismentScanner() {
}

void BleAdvertismentScanner::add(I_Listener& pListener) {
    std::lock_guard<std::mutex> lock(mMutex);
    mListeners.push_back(&pListener);
}

void BleAdvertismentScanner::remove(I_Listener& pListener) {
    std::lock_guard<std::mutex> lock(mMutex);
    mListeners.remove(&pListener);
}

void BleAdvertismentScanner::onBleSynced(I_BleController::Synced pEvent) {
    mDiscoveryAgent = mController.createDeviceDiscoveryAgent(
        [this](std::shared_ptr<Bt::Bluetooth::BleDeviceInfo> pDeviceInfo){onAdvertisment(pDeviceInfo);},
        [this](bool pSuccess){
            ESP_LOGW(TAG, "discovery ended (success = %s)", pSuccess?"true":"false");
        }
    );
    ESP_LOGI(TAG, "Start discovery for ever");
    mDiscoveryAgent->start(std::chrono::milliseconds::max());
}

void BleAdvertismentScanner::onBleReset(I_BleController::Reset pEvent) {
    ESP_LOGE(TAG, "BLR reset => delete discovery agent");
    mDiscoveryAgent = nullptr;
}

void BleAdvertismentScanner::onAdvertisment(std::shared_ptr<Bt::Bluetooth::BleDeviceInfo> pDeviceInfo) {
    std::lock_guard<std::mutex> lock(mMutex);
    for(I_Listener* listener : mListeners){
		listener->onAdvertisment(pDeviceInfo);
	}
}

} // namespace Bluetooth
} // namespace Bt

