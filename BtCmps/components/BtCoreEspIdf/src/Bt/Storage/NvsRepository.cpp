#include "Bt/Storage/NvsRepository.h"

#include <mutex>

#include "Bt/Storage/Tag.h"

namespace Bt {
namespace Storage {
namespace {
   const char* NVS_NAMESPACE = "BtNvsRepo";
   std::once_flag sNvs;
}

NvsRepository::NvsRepository() {
   std::call_once(sNvs,[]() {
      esp_err_t rc = nvs_flash_init();
      if (rc == ESP_ERR_NVS_NO_FREE_PAGES || rc == ESP_ERR_NVS_NEW_VERSION_FOUND) {
         ESP_LOGI(TAG, "nvs_flash_init failed %s => nvs_flash_erase", esp_err_to_name(rc));
         ESP_ERROR_CHECK(nvs_flash_erase());
         rc = nvs_flash_init();
      }
      ESP_ERROR_CHECK(rc);
      ESP_LOGI(TAG, "call_once => nvs_flash_init success");
   });
   ESP_ERROR_CHECK(::nvs_open(NVS_NAMESPACE, NVS_READWRITE, &mHandle));
}

NvsRepository::~NvsRepository() {

}

void NvsRepository::commit() {
   ESP_ERROR_CHECK(::nvs_commit(mHandle));
}

bool NvsRepository::set(const char* pKey, uint32_t pValue) {
   esp_err_t rc = ::nvs_set_u32(mHandle, pKey, pValue);
   if(rc == ESP_OK) {
      return true;
   }
   ESP_LOGE(TAG, "nvs_set_u32 failed %s", esp_err_to_name(rc));
   return false;
}

bool NvsRepository::tryGet(const char* pKey, uint32_t& pValue) {
   esp_err_t rc = ::nvs_get_u32(mHandle, pKey, &pValue);
   if(rc == ESP_OK) {
      return true;
   }
   ESP_LOGI(TAG, "nvs_get_u32 failed %s", esp_err_to_name(rc));
   return false;
}

//bool NvsRepository::tryGet(const char* pKey, uint8_t* bBuffer, size_t* pLength) {
//   esp_err_t rc = ::nvs_set_blob(mHandle, pKey, bBuffer, pLength);
//   if(rc == ESP_OK) {
//      return true;
//   }
//   ESP_LOGE(TAG, "nvs_get_blob failed %s", esp_err_to_name(rc));
//   return false;
//}
//
//bool NvsRepository::tryGet(const char* pKey, uint8_t* bBuffer, size_t* pLength) {
//   esp_err_t rc = ::nvs_get_blob(mHandle, pKey, bBuffer, pLength);
//   if(rc == ESP_OK) {
//      return true;
//   }
//   ESP_LOGE(TAG, "nvs_get_blob failed %s", esp_err_to_name(rc));
//   return false;
//}





} // namespace Storage
} // namespace Bt
