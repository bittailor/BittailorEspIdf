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

#include <type_traits>

namespace Bt {
namespace Core {

template <typename Enumeration>
auto asInteger(Enumeration const value) -> typename std::underlying_type<Enumeration>::type
{
   return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

} // namespace Core
} // namespace Bt


#endif // INC__Bt_Core_Logging__h

