//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::System::OtaUpdate
//
//*************************************************************************************************

#include "Bt/System/OtaUpdate.h"

#include <esp_system.h>

#include <Bt/Core/Logging.h>
#include <Bt/Core/StringUtilities.h>

#include "Bt/System/I_System.h"

namespace Bt {
namespace System {
namespace {
    constexpr const char* TAG = "Bt::Bluetooth::BleCharacteristic"; 
}

OtaUpdate::OtaUpdate(Concurrency::I_ExecutionContext& pExecutionContext, Protocols::I_MqttController& pMqttController)
: mExecutionContext(pExecutionContext), mMqttController(pMqttController)
, mUpdateSubscription(mExecutionContext, mMqttController, [this](auto&& pMessage){onMessage(pMessage);}, Core::stringPrintf("bittailor/ota/%s/data", System::getId().c_str()), 2) 
, mUpdateHandle(), mUpdatePartition(nullptr){
}

OtaUpdate::~OtaUpdate() {
}

void OtaUpdate::onMessage(std::shared_ptr<Protocols::MqttMessage> pMessage) {
    ESP_LOGI(TAG, "OTA msg %d", pMessage->data.length());
    if (pMessage->currentDataOffset == 0)
    {
        mUpdatePartition = esp_ota_get_next_update_partition(NULL);
        assert(mUpdatePartition != NULL);
        ESP_LOGI(TAG, "Writing to partition %s subtype %d at offset 0x%x", mUpdatePartition->label , mUpdatePartition->subtype, mUpdatePartition->address);
        
        int err = esp_ota_begin(mUpdatePartition, OTA_SIZE_UNKNOWN, &mUpdateHandle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
            return;
        }
        ESP_LOGI(TAG, "esp_ota_begin succeeded");
    }
    if (mUpdateHandle)
    {
        int err = esp_ota_write(mUpdateHandle, pMessage->data.c_str(), pMessage->data.length());
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_ota_write failed (%s)!", esp_err_to_name(err));
            return;
        }
    }
    if (pMessage->data.length() + pMessage->currentDataOffset >= pMessage->totalDataLength)
    {
        if (esp_ota_end(mUpdateHandle) != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_ota_end failed!");
            return;
        }
        int err = esp_ota_set_boot_partition(mUpdatePartition);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
            return;
        }
        ESP_LOGI(TAG, "Prepare to restart system!");
        esp_restart();
        return;
    }
}

} // namespace System
} // namespace Bt

