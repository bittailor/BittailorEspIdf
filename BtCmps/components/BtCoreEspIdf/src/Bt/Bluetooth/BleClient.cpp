//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleClient
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleClient.h"

#include <limits>

#include "Bt/Bluetooth/BleService.h"

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleClient";
    constexpr int32_t cConnectTimeout = 30000;   
}

BleClient::BleClient(I_Listener& pListener) : 
mListener(pListener), mAddress(), mAddressString("-"), mConnectionHandle(std::numeric_limits<uint16_t>::max()) {

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
    int rc = ble_gap_connect(ownAddrType, &address, cConnectTimeout, NULL, &BleClient::onGapEventStatic, this);
    if(rc != 0 && rc != BLE_HS_EDONE) {
        ESP_LOGW(TAG, "ble_gap_connect failed with %d", rc);
        return false;
    }
    return true;
}

bool BleClient::getService(const BleUuid& pServiceUuid) {
    ble_uuid_any_t uuid;
    uuid.u128.u.type = BLE_UUID_TYPE_128;
    pServiceUuid.to128BitLE(uuid.u128.value);
    int rc = ble_gattc_disc_svc_by_uuid(mConnectionHandle, &uuid.u, &BleClient::onServiceDiscoverStatic, this);
    if(rc != ESP_OK) {
        ESP_LOGW(TAG, "ble_gattc_disc_svc_by_uuid failed with %d", rc);
        return false;
    }
    return true;
}

int BleClient::onGapEventStatic(struct ble_gap_event* pEvent, void* pArg) {
    if(pArg!=nullptr) {
        return static_cast<BleClient*>(pArg)->onGapEvent(pEvent);    
    }
    return ESP_ERR_INVALID_ARG;
}

int BleClient::onGapEvent(struct ble_gap_event* pEvent) {
    ESP_LOGI(TAG, "[%s] onGapEvent pEvent->type=%d", mAddressString.c_str(), pEvent->type);
    switch(pEvent->type) {
        case BLE_GAP_EVENT_DISCONNECT: {
            ESP_LOGI(TAG, "[%s] BLE_GAP_EVENT_DISCONNECT %d", mAddressString.c_str(), pEvent->type);
            mListener.onDisconnect();
        } break;

        case BLE_GAP_EVENT_CONNECT: {
            ESP_LOGI(TAG, "[%s] BLE_GAP_EVENT_CONNECT %d", mAddressString.c_str(), pEvent->type);
            if(pEvent->connect.status == 0) {
                mConnectionHandle = pEvent->connect.conn_handle;
                mListener.onConnect();

            } else {
                ESP_LOGE(TAG, "[%s] error: Connection failed; status=%d",
                    mAddressString.c_str(),
                    pEvent->connect.status);
                mListener.onDisconnect();
            }
        } break;
    }
    return ESP_OK;    
}

int BleClient::onServiceDiscoverStatic(uint16_t pConnHandle, const struct ble_gatt_error* pError, const struct ble_gatt_svc* pService, void* pArg) {
    if(pArg!=nullptr) {
        return static_cast<BleClient*>(pArg)->onServiceDiscover(pConnHandle, pError, pService);    
    }
    return ESP_ERR_INVALID_ARG;
}

int BleClient::onServiceDiscover(uint16_t pConnHandle, const struct ble_gatt_error* pError, const struct ble_gatt_svc* pService) {
    
    if(mConnectionHandle != pConnHandle ) {
        ESP_LOGW(TAG, "[%s] handle mismatch mConnectionHandle[%d] != pConnHandle[%d]",mAddressString.c_str(), mConnectionHandle, pConnHandle);
        return ESP_OK;    
    }
    if(pError->status == BLE_HS_EDONE) {
        ESP_LOGI(TAG, "[%s] BLE_HS_EDONE",mAddressString.c_str());
        return ESP_OK;    
    }
    if(pError->status != ESP_OK) {
        ESP_LOGE(TAG, "[%s] service discovery failed with %d",mAddressString.c_str(), pError->status);   
        return ESP_OK;
    }
    auto service = std::make_shared<Bluetooth::BleService>(*this, *pService);
    mListener.onServiceDiscover(service);

    return ESP_OK;
}

} // namespace Bluetooth
} // namespace Bt

