//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Core::Logging
//
//*************************************************************************************************

#ifndef INC__Bt_Core_Logging__h
#define INC__Bt_Core_Logging__h

#ifdef ESP_PLATFORM
	#include "esp_log.h"
#else 

#include <stdio.h> 

const char* pathToFileName(const char* pPath);

#define log_printf printf

#define BT_LOG_FORMAT(letter, tag, format) "[" #letter "][%s:%u] %s(): [" tag "] " format "\r\n", pathToFileName(__FILE__), __LINE__, __FUNCTION__

#define ESP_LOGV(tag, format, ...)
#define ESP_LOGD(tag, format, ...)
#define ESP_LOGI(tag, format, ...)
#define ESP_LOGW(tag, format, ...)
#define ESP_LOGE(tag, format, ...)


#endif

#endif // INC__Bt_Core_Logging__h

