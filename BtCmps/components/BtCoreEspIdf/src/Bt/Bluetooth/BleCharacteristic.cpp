//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleCharacteristic
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleCharacteristic.h"

#include <map>

#include <Bt/Core/StringUtilities.h>

#include "Bt/Bluetooth/BleUuid.h"
#include "Bt/Bluetooth/BleDescriptorInfo.h"
#include "Bt/Bluetooth/BleDescriptor.h"
#include "Bt/Bluetooth/Utilities.h"


namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleCharacteristic"; 

    static const uint16_t cCharacteristicDeclaration = 0x2803;
    static const BleUuid cNotifyDescriptor = BleUuid::from16Bit(0x2902); 

    class DiscoverDescriptorsCallback {
        public:

            DiscoverDescriptorsCallback(BleCharacteristic& pBleClient, I_BleCharacteristic::OnDescriptorsDiscover pOnDescriptorsDiscover)
            : mBleClient(pBleClient), mOnDescriptorsDiscover(pOnDescriptorsDiscover), mDone(false){}

            int onDiscoverDescriptors(uint16_t pConnHandle, const ble_gatt_error* pError, uint16_t pChrValHandle, const ble_gatt_dsc* pDsc) {

                if(mBleClient.service().client().connectionHandle() != pConnHandle ) {
                    ESP_LOGW(TAG, "handle mismatch mConnectionHandle[%d] != pConnHandle[%d]", mBleClient.service().client().connectionHandle(), pConnHandle);
                    return ESP_OK;    
                }
                if(pError->status == BLE_HS_EDONE) {
                    ESP_LOGI(TAG, "BLE_HS_EDONE");
                    mOnDescriptorsDiscover(mDescriptors);
                    mDone = true;
                    return ESP_OK;    
                }
                if(pError->status != ESP_OK) {
                    ESP_LOGE(TAG, "characteristic discovery failed with %d", pError->status);   
                    return ESP_OK;
                }


                if(pDsc->uuid.u.type == BLE_UUID_TYPE_16 && pDsc->uuid.u16.value == uint16_t(cCharacteristicDeclaration)) {
                    ESP_LOGI(TAG, "UUID is Characteristic Declaration => end of Characteristic definintion");
                    mOnDescriptorsDiscover(mDescriptors);
                    mDone = true;
                    return BLE_HS_EDONE;
                }

                auto uuid = toBleUuid(pDsc->uuid);              
                auto descriptor = std::make_shared<BleDescriptor>(mBleClient, BleDescriptorInfo{pDsc->handle, uuid});
                if(!mDescriptors.insert({uuid,descriptor}).second) {
                    ESP_LOGI(TAG, "insert failed for Descriptor %d => %s ", pDsc->handle, uuid.toString().c_str()); 
                }
                ESP_LOGI(TAG, "Descriptor %d => %s", pDsc->handle, uuid.toString().c_str()); 
                return ESP_OK;
            }

        BleCharacteristic& mBleClient;
        I_BleCharacteristic::OnDescriptorsDiscover mOnDescriptorsDiscover;
        bool mDone;
        I_BleCharacteristic::Descriptors mDescriptors;       
    };

}    

BleCharacteristic::BleCharacteristic(BleService& pService, const ble_gatt_chr& pCharacteristic)
: mService(pService), mCharacteristic(pCharacteristic)  {

}

BleCharacteristic::~BleCharacteristic() {

}

std::string BleCharacteristic::toString() const {
    return Core::stringPrintf("BleService def_handle=%d val_handle=%d properties=%d Uuid=%s",  
    mCharacteristic.def_handle, mCharacteristic.val_handle, (int)mCharacteristic.properties,
    toBleUuid(mCharacteristic.uuid).toString().c_str());
}

bool BleCharacteristic::subscribe(OnSubscribe pOnSubscribe) {
    mOnSubscribe = pOnSubscribe;
    bool rc = discoverDescriptors([](const I_BleCharacteristic::Descriptors& descriptors){
        auto iter = descriptors.find(cNotifyDescriptor);
        if(iter == std::end(descriptors)) {
            ESP_LOGW(TAG, "subscribe descriptors %s not found", cNotifyDescriptor.toString().c_str());
            return;
        }
        uint8_t val[] = {0x01,0x00} ;
        iter->second->writeValue(val, 2);
    });
    if(!rc) {
        mOnSubscribe = nullptr;
        return false;
    }
    return true;
}

bool BleCharacteristic::writeValue(uint8_t* pData, size_t pLenght) {
    int rc = ble_gattc_write_no_rsp_flat(mService.client().connectionHandle(), mCharacteristic.val_handle, pData, pLenght);
    if(rc != ESP_OK) {
        ESP_LOGE(TAG, "writeValue failed with %d", rc); 
        return false;
    }
    return true;
}

void BleCharacteristic::onNotify(uint8_t* pData, size_t pLength) {
    if(mOnSubscribe) {
        mOnSubscribe(pData, pLength);
    } else {
        ESP_LOGW(TAG, "mOnSubscribe handle is nullptr");    
    }
}

bool BleCharacteristic::discoverDescriptors(OnDescriptorsDiscover pOnDescriptorsDiscover) {
    auto callback = new DiscoverDescriptorsCallback(*this, pOnDescriptorsDiscover);
    
    int rc = ble_gattc_disc_all_dscs(mService.client().connectionHandle(), mCharacteristic.val_handle, mService.endHandle(), &BleCharacteristic::onDiscoverDescriptorsStatic, callback);
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "ble_gattc_disc_all_dscs failed with %d", rc);
        delete callback;
        return false;
    }
    return true;
}

int BleCharacteristic::onDiscoverDescriptorsStatic(uint16_t pConnHandle, const ble_gatt_error* pError, uint16_t pChrValHandle, const ble_gatt_dsc* pDsc, void* pArg) {
    ESP_LOGI(TAG, "onDiscoverDescriptorsStatic");
    if(pArg!=nullptr) {
        auto callback = static_cast<DiscoverDescriptorsCallback*>(pArg);
        int rc = callback->onDiscoverDescriptors(pConnHandle, pError, pChrValHandle, pDsc);
        if(callback->mDone) {
            delete callback;
        }    
        return rc;
    }
    return ESP_ERR_INVALID_ARG;
}





} // namespace Bluetooth
} // namespace Bt

