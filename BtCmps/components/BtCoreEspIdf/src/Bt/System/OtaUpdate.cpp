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
    constexpr const char* TAG = "Bt::System::OtaUpdate"; 
}

using namespace std::chrono_literals;

OtaUpdate::OtaUpdate(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Protocols::I_MqttController& pMqttController)
: mExecutionContext(pExecutionContext), mMqttController(pMqttController)
, mUpdateSubscription(mMqttController, Core::stringPrintf("bittailor/ota/%s/data",System::getId().c_str()), 2, [this](const Protocols::RawMqttMessage& pMessage){onMessage(pMessage);})  
, mRebootSubscription(pExecutionContext, mMqttController, Core::stringPrintf("bittailor/ota/%s/restart",System::getId().c_str()), 2, [this](std::shared_ptr<Protocols::MqttMessage> pMessage){onRestartMessage(pMessage);})  
, mUpdateHandle(), mUpdatePartition(nullptr){
}

OtaUpdate::~OtaUpdate() {
}

void OtaUpdate::onMessage(const Protocols::RawMqttMessage& pMessage) {
    if (pMessage.currentDataOffset == 0)
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
        int err = esp_ota_write(mUpdateHandle, pMessage.data, pMessage.dataLength);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_ota_write failed (%s)!", esp_err_to_name(err));
            return;
        }
        ESP_LOGI(TAG, "ota %3d%%", ( (100*(pMessage.currentDataOffset+pMessage.dataLength))/pMessage.totalDataLength));
    }
    if (pMessage.dataLength + pMessage.currentDataOffset >= pMessage.totalDataLength)
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
        ESP_LOGI(TAG, "OTA update successfully written");
        ESP_LOGW(TAG, "*** RESTART IN 1 SECOND ***");
        mExecutionContext.callOnce(1s,[](){
            esp_restart();
        });
        return;
    }
}

void OtaUpdate::onRestartMessage(std::shared_ptr<Protocols::MqttMessage> pMessage) {
    if(pMessage->data == "restart") {
        ESP_LOGI(TAG, "RESTART msg receievd!");
        ESP_LOGW(TAG, "*** RESTART IN 1 SECOND ***");
        mExecutionContext.callOnce(1s,[](){
            esp_restart();
        });
        return;
    }
    ESP_LOGW(TAG, "Invalid message RESTART msg: %s", pMessage->data.c_str());    
}

} // namespace System
} // namespace Bt

