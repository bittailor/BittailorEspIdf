//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Bluetooth::BleDeviceDiscoveryAgent
//
//*************************************************************************************************

#include "Bt/Bluetooth/BleDeviceDiscoveryAgent.h"

#include <Bt/Core/StringBuilder.h>
#include <Bt/Bluetooth/BleUuid.h>
#include <Bt/Bluetooth/BleDeviceInfo.h>


namespace Bt {
namespace Bluetooth {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleDeviceDiscoveryAgent";
}

BleDeviceDiscoveryAgent::BleDeviceDiscoveryAgent(OnDiscover pOnDiscover , OnDiscoverComplete pOnDiscoverComplete) 
: mOnDiscover(pOnDiscover), mOnDiscoverComplete(pOnDiscoverComplete), mParameters{} {
    ESP_ERROR_CHECK(ble_hs_id_infer_auto(0, &mOwnAddrType));
    mParameters.filter_policy      = BLE_HCI_SCAN_FILT_NO_WL;
    mParameters.passive            = 1; // If set, don’t send scan requests to advertisers (i.e., don’t request additional advertising data).
    mParameters.itvl               = 0; // This is defined as the time interval from when the Controller started its last LE scan until it begins the subsequent LE scan. (units=0.625 msec)
    mParameters.window             = 0; // The duration of the LE scan. LE_Scan_Window shall be less than or equal to LE_Scan_Interval (units=0.625 msec)
    mParameters.limited            = 0; // If set, only discover devices in limited discoverable mode.
    mParameters.filter_duplicates  = 0; // If set, the controller ignores all but the first advertisement from each device.

}

BleDeviceDiscoveryAgent::~BleDeviceDiscoveryAgent() {
}

void BleDeviceDiscoveryAgent::start(std::chrono::milliseconds pDuration) {
    int32_t duration = pDuration.count();
    if(pDuration == pDuration.max()) {
        duration = BLE_HS_FOREVER;
    }
    ESP_LOGI(TAG, "ble_gap_disc for %d", duration);
    ESP_ERROR_CHECK(ble_gap_disc(mOwnAddrType, duration, &mParameters, &BleDeviceDiscoveryAgent::onGapEventStatic, this));
}

int BleDeviceDiscoveryAgent::onGapEventStatic(ble_gap_event* pEvent, void* pArg) {
    if(pArg!=nullptr) {
        return static_cast<BleDeviceDiscoveryAgent*>(pArg)->onGapEvent(pEvent);    
    }
    return ESP_ERR_INVALID_ARG;
}

int BleDeviceDiscoveryAgent::onGapEvent(ble_gap_event* pEvent) {
    ESP_LOGD(TAG, "onGapEvent pEvent->type=%d",pEvent->type);   
    switch (pEvent->type)
    {
        case BLE_GAP_EVENT_DISC: {
            ESP_LOGD(TAG, "onGapEvent BLE_GAP_EVENT_DISC");
            onDiscover(pEvent);
        } break;
        case BLE_GAP_EVENT_DISC_COMPLETE: {
            ESP_LOGD(TAG, "onGapEvent BLE_GAP_EVENT_DISC_COMPLETE");
            onDiscoverComplete(pEvent);
        } break;
    }    
    return ESP_OK;
}

void BleDeviceDiscoveryAgent::onDiscover(ble_gap_event* pEvent) {
    auto address = BleAddress::from48BitLe(pEvent->disc.addr.val, pEvent->disc.addr.type);
    
    auto iter = mDiscoveredDevices.find(address);
    std::shared_ptr<BleDeviceInfo> deviceInfo;
    if(iter != std::end(mDiscoveredDevices)) {
        deviceInfo = iter->second;
    } else {
        deviceInfo = std::make_shared<BleDeviceInfo>();
        deviceInfo->address(address);    
    }
    
    ble_hs_adv_fields fields;
    int rc = ble_hs_adv_parse_fields(&fields, pEvent->disc.data, pEvent->disc.length_data);
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_hs_adv_parse_fields failed with %d", rc);
    }

    // if (fields.uuids16 != NULL) {
    //     ESP_LOGI(TAG, " - uuids16(%scomplete)=", fields.uuids16_is_complete ? "" : "in");
    //     for (int i = 0; i < fields.num_uuids16; i++) {
    //         ESP_LOGI(TAG, "  -- 0x%04" PRIx16, fields.uuids16[i].value);
    //     }
    // }   
    // if (fields.uuids32 != NULL) {
    //     ESP_LOGI(TAG, " - uuids32(%scomplete)=",fields.uuids32_is_complete ? "" : "in");
    //     for (int i = 0; i < fields.num_uuids32; i++) {
    //         ESP_LOGI(TAG, "  -- 0x%08" PRIx32, fields.uuids32[i].value);
    //     }
    // }

    // if (fields.uuids128 != NULL) {
    //     ESP_LOGI(TAG, "    uuids128(%scomplete)=", fields.uuids128_is_complete ? "" : "in");
    //     for (int i = 0; i < fields.num_uuids128; i++) {
    //         stringBuilder.reset();
    //         stringBuilder.hexencode(fields.uuids128[i].value, sizeof(fields.uuids128[i].value)/sizeof(fields.uuids128[i].value[0]));
    //         ESP_LOGI(TAG, "  -- 0x%s", stringBuilder.c_str());
    //     }
    // }

    if (fields.svc_data_uuid16 != NULL) {
        deviceInfo->serviceData(BleUuid::from16BitLe(fields.svc_data_uuid16), 
            fields.svc_data_uuid16+BleUuid::cNumBytes16, 
            fields.svc_data_uuid16_len-BleUuid::cNumBytes16);
    }

    if (fields.svc_data_uuid32 != NULL) {
        deviceInfo->serviceData(BleUuid::from32BitLe(fields.svc_data_uuid32), 
            fields.svc_data_uuid32+BleUuid::cNumBytes32, 
            fields.svc_data_uuid32_len-BleUuid::cNumBytes32);
    }

    if (fields.svc_data_uuid128 != NULL) {
        deviceInfo->serviceData(BleUuid::from128BitLE(fields.svc_data_uuid128), 
            fields.svc_data_uuid128+BleUuid::cNumBytes128, 
            fields.svc_data_uuid128_len-BleUuid::cNumBytes128);
    }
    mDiscoveredDevices.insert({deviceInfo->address(),deviceInfo});
    if(mOnDiscover) {
        mOnDiscover(deviceInfo); 
    } 
}

void BleDeviceDiscoveryAgent::onDiscoverComplete(ble_gap_event* pEvent) {
    int reason = pEvent->disc_complete.reason;
    if(reason != 0) {
        ESP_LOGE(TAG, "discovery completed with failure reason: %d", reason);
    }
    std::vector<std::shared_ptr<BleDeviceInfo>> discoveredDevices;
    std::transform(std::begin(mDiscoveredDevices),std::end(mDiscoveredDevices), 
        std::back_inserter(discoveredDevices),
        [](auto iter){return iter.second;});

    if(mOnDiscoverComplete) {
        mOnDiscoverComplete(discoveredDevices); 
    } 
}


} // namespace Bluetooth
} // namespace Bt

