//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleService
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleService.h"

#include <Bt/Core/StringUtilities.h>
#include <Bt/Bluetooth/BleUuid.h>
#include <Bt/Bluetooth/Utilities.h>

#include "Bt/Bluetooth/BleCharacteristic.h"

namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleService"; 

    class CharacteristicDiscoverCallback{
        public:

            CharacteristicDiscoverCallback(BleService& pBleService, I_BleService::OnCharacteristicDiscover pOnCharacteristicDiscover)
            : mBleService(pBleService), mOnCharacteristicDiscover(pOnCharacteristicDiscover), mDone(false)  {}

            int onCharacteristicDiscover(uint16_t pConnHandle, const ble_gatt_error* pError, const ble_gatt_chr* pCharacteristic) {
    
                if(mBleService.client().connectionHandle() != pConnHandle ) {
                    ESP_LOGW(TAG, "handle mismatch mConnectionHandle[%d] != pConnHandle[%d]", mBleService.client().connectionHandle(), pConnHandle);
                    return ESP_OK;    
                }
                if(pError->status == BLE_HS_EDONE) {
                    ESP_LOGI(TAG, "BLE_HS_EDONE");
                    mDone = true;
                    mOnCharacteristicDiscover(mCharacteristic);
                    return ESP_OK;    
                }
                if(pError->status != ESP_OK) {
                    ESP_LOGE(TAG, "characteristic discovery failed with %d", pError->status);   
                    return ESP_OK;
                }
                mCharacteristic = std::make_shared<Bluetooth::BleCharacteristic>(mBleService, *pCharacteristic);
                return ESP_OK;
            }   

            BleService& mBleService;
            I_BleService::OnCharacteristicDiscover mOnCharacteristicDiscover;
            bool mDone;
            I_BleService::BleCharacteristic mCharacteristic;

    };

}

BleService::BleService(BleClient& pClient, const ble_gatt_svc& pService)
: mClient(pClient), mService(pService) {

}

BleService::~BleService() {

}

bool BleService::getCharacteristic(const BleUuid& pCharacteristicUuid, OnCharacteristicDiscover pOnCharacteristicDiscover) {
    ble_uuid_any_t uuid;
    uuid.u128.u.type = BLE_UUID_TYPE_128;
    pCharacteristicUuid.to128BitLE(uuid.u128.value);
    auto callback = new CharacteristicDiscoverCallback(*this, pOnCharacteristicDiscover);
    int rc = ble_gattc_disc_chrs_by_uuid(mClient.connectionHandle(), mService.start_handle, mService.end_handle, &uuid.u, &BleService::onCharacteristicDiscoverStatic, callback);
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "ble_gattc_disc_chrs_by_uuid failed with %d", rc);
        delete callback;
        return false;
    }
    return true;
}

std::string BleService::toString() const {
    return Core::stringPrintf("BleService startHandle=%d endHandle=%d Uuid=%s",  
    mService.start_handle, mService.end_handle,
    toBleUuid(mService.uuid).toString().c_str());        
}

int BleService::onCharacteristicDiscoverStatic(uint16_t pConnHandle, const ble_gatt_error* pError, const ble_gatt_chr* pCharacteristic, void* pArg) {
    ESP_LOGI(TAG, "onCharacteristicDiscoverStatic");
    if(pArg!=nullptr) {
        auto callback = static_cast<CharacteristicDiscoverCallback*>(pArg);
        int rc = callback->onCharacteristicDiscover(pConnHandle, pError, pCharacteristic); 
        if(callback->mDone) {
            delete callback;
        }   
        return rc;
    }
    return ESP_ERR_INVALID_ARG;
}



} // namespace Bluetooth
} // namespace Bt

