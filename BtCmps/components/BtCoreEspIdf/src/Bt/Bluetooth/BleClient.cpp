//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleClient
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleClient.h"

#include <limits>

#include <Bt/Core/StringBuilder.h>

#include "Bt/Bluetooth/BleCharacteristic.h"
#include "Bt/Bluetooth/BleController.h"
#include "Bt/Bluetooth/BleService.h"

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleClient";
    constexpr int32_t cConnectTimeout = 30000; 

    class ServiceDiscoverCallback {
        public:

            ServiceDiscoverCallback(BleClient& pBleClient, I_BleClient::OnServiceDiscover pOnOnServiceDiscover): mBleClient(pBleClient), mOnOnServiceDiscover(pOnOnServiceDiscover), mDone(false){}

            int onServiceDiscover(uint16_t pConnHandle, const ble_gatt_error* pError, const ble_gatt_svc* pService) {
        
                if(mBleClient.connectionHandle() != pConnHandle ) {
                    ESP_LOGW(TAG, "[%s] handle mismatch mConnectionHandle[%d] != pConnHandle[%d]", mBleClient.addressString().c_str(), mBleClient.connectionHandle(), pConnHandle);
                    return ESP_OK;   
                }
                if(pError->status == BLE_HS_EDONE) {
                    ESP_LOGI(TAG, "[%s] BLE_HS_EDONE", mBleClient.addressString().c_str());
                    mOnOnServiceDiscover(mService);
                    mDone = true;
                    return ESP_OK;       
                }
                if(pError->status != ESP_OK) {
                    ESP_LOGE(TAG, "[%s] service discovery failed with %d", mBleClient.addressString().c_str(), pError->status);   
                    return pError->status;
                }
                mService = mBleClient.createService(*pService);
                return ESP_OK;
            }

            BleClient& mBleClient;
            I_BleClient::OnServiceDiscover mOnOnServiceDiscover;
            bool mDone;
            I_BleClient::BleServicePtr mService;
    };  
}

BleClient::BleClient(BleController& pController, I_Listener& pListener) 
: mController(pController), mListener(pListener)
, mAddress(), mAddressString("-")
, mClientState(ClientState::DISCONNECTED)
, mConnectionHandle(std::numeric_limits<uint16_t>::max()) {

}

BleClient::~BleClient() {

}

bool BleClient::connect(const BleAddress& pAddress) {
    mAddress = pAddress;
    mAddressString = mAddress.toString();
    
    uint8_t ownAddrType;
    ESP_ERROR_CHECK(ble_hs_id_infer_auto(0, &ownAddrType));
    ble_addr_t address;
    pAddress.to(address);
    if(mClientState != ClientState::DISCONNECTED) {
        ESP_LOGW(TAG, "[%s] client not DISCONNECTED", mAddressString.c_str());
        return false;
    }
    mClientState = ClientState::CONNECTING;
    mController.enqueConnect([this,ownAddrType,address](){
        int rc = ble_gap_connect(ownAddrType, &address, cConnectTimeout, NULL, &BleClient::onGapEventStatic, this);
        if(rc != 0 && rc != BLE_HS_EDONE) {
            ESP_LOGW(TAG, "[%s] ble_gap_connect failed with %d",mAddressString.c_str(), rc);
            mController.dequeConnect();
            mListener.onDisconnect();   
        }
    });
    return true;
}

bool BleClient::updateConnection(const ConnectionUpdateParameters& pParameters) {
    ble_gap_upd_params params;
    params.itvl_min = ((pParameters.mConnectionIntervalMinimum * 8)/10).count(); 
    params.itvl_max = ((pParameters.mConnectionIntervalMaximum * 8)/10).count();
    params.latency = pParameters.mConnectionLatency;
    params.supervision_timeout = (pParameters.mSupervisionTimeout/10).count();
    params.min_ce_len = 0x0000;
    params.max_ce_len = 0x0000;
    ESP_LOGI(TAG, "[%s] ble_gap_update_params min=%d max=%d latency=%d supervision_timeout=%d", mAddressString.c_str(),
             params.itvl_min,  params.itvl_max, params.latency, params.supervision_timeout);
    int rc = ble_gap_update_params(mConnectionHandle,&params);
    if(rc != ESP_OK) {
        ESP_LOGW(TAG, "[%s]ble_gap_update_params failed with %d", mAddressString.c_str(), rc);
        return false;
    }
    return true;
}
      

bool BleClient::getService(const BleUuid& pServiceUuid, OnServiceDiscover pOnOnServiceDiscover) {
    ble_uuid_any_t uuid;
    uuid.u128.u.type = BLE_UUID_TYPE_128;
    pServiceUuid.to128BitLE(uuid.u128.value);
    auto callback = new ServiceDiscoverCallback(*this, pOnOnServiceDiscover);
    int rc = ble_gattc_disc_svc_by_uuid(mConnectionHandle, &uuid.u, &BleClient::onServiceDiscoverStatic, callback);
    if(rc != ESP_OK) {
        ESP_LOGW(TAG, "[%s]ble_gattc_disc_svc_by_uuid failed with %d", mAddressString.c_str(), rc);
        delete callback;
        return false;
    }
    return true;
}

I_BleClient::BleServicePtr BleClient::createService(const ble_gatt_svc& pService) {
    auto service = std::make_shared<BleService>(*this, pService);
    mServices.insert({service->uuid(), service});
    return service;
}

I_BleService::BleCharacteristicPtr BleClient::createCharacteristic(BleService& pService, const ble_gatt_chr& pCharacteristic) {
    auto characteristic = std::make_shared<Bluetooth::BleCharacteristic>(pService, pCharacteristic); 
    ESP_LOGI(TAG, "[%s] Insert BleCharacteristic handle %d", mAddressString.c_str(), characteristic->valueHandle());
    mCharacteristics.insert({characteristic->valueHandle(),characteristic});
    return characteristic;
}

int BleClient::onGapEventStatic(ble_gap_event* pEvent, void* pArg) {
    if(pArg!=nullptr) { 
        return static_cast<BleClient*>(pArg)->onGapEvent(pEvent);    
    }
    return ESP_ERR_INVALID_ARG;
}

int BleClient::onGapEvent(ble_gap_event* pEvent) {
    ESP_LOGD(TAG, "[%s] onGapEvent pEvent->type=%d", mAddressString.c_str(), pEvent->type);
    switch(pEvent->type) {
        case BLE_GAP_EVENT_DISCONNECT: {
            ESP_LOGI(TAG, "[%s] BLE_GAP_EVENT_DISCONNECT %d", mAddressString.c_str(), pEvent->type);
            if(mClientState == ClientState::CONNECTING) {
                mController.dequeConnect();     
            }
            mClientState = ClientState::DISCONNECTED;
            mListener.onDisconnect();
            return ESP_OK;
        } break;

        case BLE_GAP_EVENT_CONNECT: {
            ESP_LOGI(TAG, "[%s] BLE_GAP_EVENT_CONNECT %d conn_handle = %d", mAddressString.c_str(), pEvent->type, pEvent->connect.conn_handle);
            if(pEvent->connect.status == 0) {
                mConnectionHandle = pEvent->connect.conn_handle;
                mClientState = ClientState::CONNECTED;
                mListener.onConnect();
                
            } else {
                ESP_LOGE(TAG, "[%s] error: Connection failed; status=%d", mAddressString.c_str(), pEvent->connect.status);
                mClientState = ClientState::DISCONNECTED;
                mListener.onDisconnect();
            }
            mController.dequeConnect();
            return ESP_OK;
        } break;

        case BLE_GAP_EVENT_NOTIFY_RX: {
            if(mConnectionHandle != pEvent->notify_rx.conn_handle) {
                return 0;
            }
                
            ESP_LOGD(TAG, "[%s] BLE_GAP_EVENT_NOTIFY_RX %d", mAddressString.c_str(), pEvent->type);
            ESP_LOGD(TAG, "Notify Recieved for handle: %d",pEvent->notify_rx.attr_handle);
            ESP_LOGD(TAG, "  Data: %s", Core::DefaultStringBuilder().hexencode(pEvent->notify_rx.om->om_data,pEvent->notify_rx.om->om_len).c_str());
            auto iter = mCharacteristics.find(pEvent->notify_rx.attr_handle);
            if(iter == std::end(mCharacteristics)) {
                ESP_LOGW(TAG, "[%s] error: handle %d not found ",mAddressString.c_str(), pEvent->notify_rx.attr_handle);
                return ESP_OK;
            }
            iter->second->onNotify(pEvent->notify_rx.om->om_data,pEvent->notify_rx.om->om_len);



            return ESP_OK;
        }break;

        case BLE_GAP_EVENT_L2CAP_UPDATE_REQ: {
            ESP_LOGI(TAG, "[%s] Peer requesting to update connection parameters => just accept them", mAddressString.c_str());
            return ESP_OK;
        }break;

         case BLE_GAP_EVENT_CONN_UPDATE: {
            if (pEvent->conn_update.status == 0) {
                ESP_LOGI(TAG, "[%s] connection was successfully updated.", mAddressString.c_str());
            } else {
                ESP_LOGW(TAG, "[%s] connection update failed with 0x%x", mAddressString.c_str(), pEvent->conn_update.status);    
            }            
            return ESP_OK;
        }break;


        default: {
            ESP_LOGW(TAG, "[%s] onGapEvent pEvent->type=%d ignored", mAddressString.c_str(), pEvent->type);
        }break;    
    }
    return ESP_OK;    
}

int BleClient::onServiceDiscoverStatic(uint16_t pConnHandle, const ble_gatt_error* pError, const ble_gatt_svc* pService, void* pArg) {
    if(pArg!=nullptr) {
        auto callback = static_cast<ServiceDiscoverCallback*>(pArg);
        int rc = callback->onServiceDiscover(pConnHandle, pError, pService);
        if(callback->mDone) {
            delete callback;
        }
        return rc;

    }
    return ESP_ERR_INVALID_ARG;
}

} // namespace Bluetooth
} // namespace Bt

