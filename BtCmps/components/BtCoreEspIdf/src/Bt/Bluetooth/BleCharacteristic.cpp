//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleCharacteristic
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleCharacteristic.h"

#include <Bt/Core/StringUtilities.h>
#include <Bt/Bluetooth/BleUuid.h>

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleCharacteristic"; 
}    

BleCharacteristic::BleCharacteristic(BleService& pService, const ble_gatt_chr& pCharacteristic)
: mService(pService), mCharacteristic(pCharacteristic)  {

}

BleCharacteristic::~BleCharacteristic() {

}

std::string BleCharacteristic::toString() const {
    return Core::stringPrintf("BleService def_handle=%d val_handle=%d properties=%d Uuid=%s",  
    mCharacteristic.def_handle, mCharacteristic.val_handle, (int)mCharacteristic.properties, 
    BleUuid::from128BitLE(mCharacteristic.uuid.u128.value).toString().c_str());
}

bool BleCharacteristic::subscribe(OnSubscribe pOnSubscribe) {
    return discoverDescriptors();

}

bool BleCharacteristic::discoverDescriptors() {
    int rc = ble_gattc_disc_all_dscs(mService.client().connectionHandle(), mCharacteristic.val_handle, mService.endHandle(), &BleCharacteristic::onDiscoverDescriptorsStatic, this);
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "ble_gattc_disc_all_dscs failed with %d", rc);
        return false;
    }
    return true;
}

int BleCharacteristic::onDiscoverDescriptorsStatic(uint16_t pConnHandle, const ble_gatt_error* pError, uint16_t pChrValHandle, const ble_gatt_dsc* pDsc, void* pArg) {
    ESP_LOGI(TAG, "onDiscoverDescriptorsStatic");
    if(pArg!=nullptr) {
        return static_cast<BleCharacteristic*>(pArg)->onDiscoverDescriptors(pConnHandle, pError, pChrValHandle, pDsc);    
    }
    return ESP_ERR_INVALID_ARG;
}

int BleCharacteristic::onDiscoverDescriptors(uint16_t pConnHandle, const ble_gatt_error* pError, uint16_t pChrValHandle, const ble_gatt_dsc* pDsc) {

     if(mService.client().connectionHandle() != pConnHandle ) {
        ESP_LOGW(TAG, "handle mismatch mConnectionHandle[%d] != pConnHandle[%d]", mService.client().connectionHandle(), pConnHandle);
        return ESP_OK;    
    }
    if(pError->status == BLE_HS_EDONE) {
        ESP_LOGI(TAG, "BLE_HS_EDONE");
        //mService.client().listener().onCharacteristicDiscover(characteristic); 
        return ESP_OK;    
    }
    if(pError->status != ESP_OK) {
        ESP_LOGE(TAG, "characteristic discovery failed with %d", pError->status);   
        return ESP_OK;
    }



    ESP_LOGI(TAG, "Descriptor %d => %s", pDsc->handle, BleUuid::from128BitLE(pDsc->uuid.u128.value).toString().c_str()); 

    /*
    struct ble_gatt_dsc {
        uint16_t handle;
        ble_uuid_any_t uuid;
    };
    */

     
    
    
    return ESP_OK;
}



} // namespace Bluetooth
} // namespace Bt

