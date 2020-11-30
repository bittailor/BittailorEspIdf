//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleClient
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleClient.h"

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleClient";
    constexpr int32_t cConnectTimeout = 30000;   
}

BleClient::BleClient() : mAddress(), mAddressString("-") {

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

        } break;

        case BLE_GAP_EVENT_CONNECT: {
            ESP_LOGI(TAG, "[%s]  BLE_GAP_EVENT_CONNECT %d", mAddressString.c_str(), pEvent->type);
            if(pEvent->connect.status == 0) {

            } else {
                ESP_LOGE(TAG, "[%s] error: Connection failed; status=%d",
                    mAddressString.c_str(),
                    pEvent->connect.status);
            }

        } break;

    }
    return ESP_OK;    
}
      


} // namespace Bluetooth
} // namespace Bt

