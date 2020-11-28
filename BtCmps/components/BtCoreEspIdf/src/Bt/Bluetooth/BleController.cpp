//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleController
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleController.h"

#include <atomic> 
#include <stdexcept>

#include <esp_nimble_hci.h>
#include <host/ble_hs.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>

#undef max
#undef min

#include <Bt/Core/Logging.h>
#include <Bt/Events/Events.h>

extern "C" void ble_store_config_init(void);

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleController";

    std::atomic<BleController*> sInstance{nullptr};

    void blecent_host_task(void *param)
    {
        ESP_LOGI(TAG, "BLE Host Task Started");
        nimble_port_run();
        nimble_port_freertos_deinit();
    }

}

BleController::BleController() {
    BleController* expected = nullptr;
    if(!sInstance.compare_exchange_strong(expected,this)) {
        ESP_LOGE(TAG, "Only one BleController can be instantiated 0x%p", expected);
        throw std::logic_error("Only one BleController can be instantiated"); 
    } 
    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());
    nimble_port_init();
    ble_hs_cfg.reset_cb = &BleController::onHostResetStatic;
    ble_hs_cfg.sync_cb = &BleController::onHostAndControllerSyncedStatic;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_store_config_init();
    nimble_port_freertos_init(blecent_host_task);
    
}

BleController::~BleController() {
}

void BleController::onHostReset(int pReason) {
}

void BleController::onHostAndControllerSynced() {
    Events::publish(I_BleController::Synced{});
}

void BleController::onHostResetStatic(int pReason) {
    ESP_LOGE(TAG, "onHostReset; reason=%d", pReason);
    BleController* instance = sInstance;
    if(instance!=nullptr) {
        instance->onHostReset(pReason);
    }
}

void BleController::onHostAndControllerSyncedStatic() {
    ESP_LOGI(TAG, "BLE host and controller synced");
    BleController* instance = sInstance;
    if(instance!=nullptr) {
        instance->onHostAndControllerSynced();
    }    
}

} // namespace Bluetooth
} // namespace Bt

