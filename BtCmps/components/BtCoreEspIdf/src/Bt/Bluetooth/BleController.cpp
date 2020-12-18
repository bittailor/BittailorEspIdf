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

#include "Bt/Bluetooth/BleDeviceDiscoveryAgent.h"

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

BleController::BleController(Concurrency::I_ExecutionContext& pExecutionContext)
: mState(State::OUT_OF_SYNC), mExecutionContext(pExecutionContext) {
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

std::shared_ptr<I_BleDeviceDiscoveryAgent>  BleController::createDeviceDiscoveryAgent(I_BleDeviceDiscoveryAgent::OnDiscover pOnDiscover, I_BleDeviceDiscoveryAgent::OnDiscoverComplete pOnDiscoverComplete) {
    return  std::make_shared<BleDeviceDiscoveryAgent>(pOnDiscover, pOnDiscoverComplete);
}

std::shared_ptr<I_BleClient>  BleController::createClient(I_BleClient::I_Listener& pListener) {
    mClients.remove_if([](auto&& client){return client.expired();});
    auto client =  std::make_shared<BleClient>(*this, pListener);
    mClients.push_back(client);
    return client;

}

void BleController::enqueConnect(std::function<void()> pConnect){
    mExecutionContext.ensureCallOnContext([this,pConnect](){  
        bool empty = mConnectQueue.empty();
        mConnectQueue.push(pConnect);
        if(mState == State::OUT_OF_SYNC) {
            ESP_LOGI(TAG, "connect while OUT_OF_SYNC => just queued it, queue.size = %zu", mConnectQueue.size());
            return;
        }
        if(empty) {
            ESP_LOGI(TAG, "mConnectQueue empty run connect immediately ");
            mConnectQueue.front()();
        } else {
            ESP_LOGI(TAG, "mConnectQueue not empty enque connect queue.size = %zu", mConnectQueue.size());    
        }
    });
}

void BleController::dequeConnect() {
    mExecutionContext.ensureCallOnContext([this](){   
        mConnectQueue.pop();
        if(!mConnectQueue.empty()){
            ESP_LOGI(TAG, "mConnectQueue not empty run next connect queue.size = %zu", mConnectQueue.size());
            mConnectQueue.front()();    
        }    
    });
}

void BleController::onHostReset(int pReason) {
    mExecutionContext.ensureCallOnContext([this](){
        mState = State::OUT_OF_SYNC;
        for (auto &&weakClient : mClients)
        {
            if (auto client = weakClient.lock()) {
                ESP_LOGI(TAG, "send disconnect because of reset to %s", client->addressString().c_str());
                client->listener().onDisconnect();
            }
        }
        mClients.clear();
    });        
}

void BleController::onHostAndControllerSynced() {
    mExecutionContext.ensureCallOnContext([this](){
        mState = State::IN_SYNC;
        if(!mConnectQueue.empty()) {
            ESP_LOGI(TAG, "connect queue not empty on IN_SYNC => run connect, queue.size = %zu", mConnectQueue.size());
            mConnectQueue.front()();
        }
        Events::publish(I_BleController::Synced{});
    });
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

