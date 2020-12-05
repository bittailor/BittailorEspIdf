//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleService
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleService.h"

#include <Bt/Core/StringUtilities.h>
#include <Bt/Bluetooth/BleUuid.h>

#include "Bt/Bluetooth/BleCharacteristic.h"

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleService"; 
}

BleService::BleService(BleClient& pClient, const ble_gatt_svc& pService)
: mClient(pClient), mService(pService) {

}

BleService::~BleService() {

}

bool BleService::getCharacteristic(const BleUuid& pCharacteristicUuid) {
    ble_uuid_any_t uuid;
    uuid.u128.u.type = BLE_UUID_TYPE_128;
    pCharacteristicUuid.to128BitLE(uuid.u128.value);
    int rc = ble_gattc_disc_chrs_by_uuid(mClient.connectionHandle(), mService.start_handle, mService.end_handle, &uuid.u, &BleService::onCharacteristicDiscoverStatic, this);
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "ble_gattc_disc_chrs_by_uuid failed with %d", rc);
        return false;
    }
    return true;
}

/*
uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, const ble_uuid_t *uuid, ble_gatt_chr_fn *cb, void *cb_arg
*/

std::string BleService::toString() const {
    return Core::stringPrintf("BleService startHandle=%d endHandle=%d Uuid=%s",  
    mService.start_handle, mService.end_handle, 
    BleUuid::from128BitLE(mService.uuid.u128.value).toString().c_str());        
}

int BleService::onCharacteristicDiscoverStatic(uint16_t pConnHandle, const ble_gatt_error* pError, const ble_gatt_chr* pCharacteristic, void* pArg) {
    ESP_LOGI(TAG, "onCharacteristicDiscoverStatic");
    if(pArg!=nullptr) {
        return static_cast<BleService*>(pArg)->onCharacteristicDiscover(pConnHandle, pError, pCharacteristic);    
    }
    return ESP_ERR_INVALID_ARG;
}

int BleService::onCharacteristicDiscover(uint16_t pConnHandle, const ble_gatt_error* pError, const ble_gatt_chr* pCharacteristic) {
    
    if(mClient.connectionHandle() != pConnHandle ) {
        ESP_LOGW(TAG, "handle mismatch mConnectionHandle[%d] != pConnHandle[%d]", mClient.connectionHandle(), pConnHandle);
        return ESP_OK;    
    }
    if(pError->status == BLE_HS_EDONE) {
        ESP_LOGI(TAG, "BLE_HS_EDONE");
        return ESP_OK;    
    }
    if(pError->status != ESP_OK) {
        ESP_LOGE(TAG, "characteristic discovery failed with %d", pError->status);   
        return ESP_OK;
    }
    auto characteristic = std::make_shared<Bluetooth::BleCharacteristic>(*this, *pCharacteristic);
    mClient.listener().onCharacteristicDiscover(characteristic);
    return ESP_OK;
}


} // namespace Bluetooth
} // namespace Bt

