//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Storage::VirtualFilesystem
//
//*************************************************************************************************

#include "Bt/Storage/VirtualFilesystem.h"

#include <stdexcept>

#include <esp_spiffs.h>

#include "Bt/Storage/Tag.h"

namespace Bt {
namespace Storage {

VirtualFilesystem::VirtualFilesystem() {

}

VirtualFilesystem::~VirtualFilesystem() {

}


void VirtualFilesystem::registerSpiffsPartition(const char* pBasePath, const char* pPartitionLabel, bool formatIfMountFailed, size_t pMaxFiles) {
   esp_vfs_spiffs_conf_t configSpiffPartition = {
            .base_path = pBasePath,
            .partition_label = pPartitionLabel,
            .max_files = pMaxFiles,
            .format_if_mount_failed = formatIfMountFailed
   };

   esp_err_t ret = esp_vfs_spiffs_register(&configSpiffPartition);

   if (ret != ESP_OK) {
      if (ret == ESP_FAIL) {
         ESP_LOGE(TAG, "Failed to mount filesystem for spiffs");
      } else if (ret == ESP_ERR_NOT_FOUND) {
         ESP_LOGE(TAG, "Failed to find partition for spiffs");
      } else {
         ESP_LOGE(TAG, "Failed to initialize spiffs (%s)", esp_err_to_name(ret));
      }
      throw std::runtime_error("registerSpiffsPartition failed");
   }

   size_t total = 0, used = 0;
   ret = esp_spiffs_info(pPartitionLabel, &total, &used);
   if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to get spiffs partition information (%s)", esp_err_to_name(ret));
   } else {
      ESP_LOGI(TAG, "spiffs partition '%s' info size: total: %d, used: %d",pPartitionLabel , total, used);
   }
}


} // namespace Storage
} // namespace Bt
