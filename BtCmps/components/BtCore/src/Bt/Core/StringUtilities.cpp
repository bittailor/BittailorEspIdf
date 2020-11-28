//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::StringUtilities
//
//*************************************************************************************************

#include "Bt/Core/StringUtilities.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "Bt/Core/Logging.h"
#include "Bt/Core/StringBuilder.h"

namespace Bt {
namespace Core {
namespace {
   const char* TAG = "StringUtilities";
}

void stringAppendV(std::string* dst, const char* format, va_list ap) __attribute__ ((format (printf, 2, 0)));    

std::string stringPrintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  std::string result;
  stringAppendV(&result, format, ap);
  va_end(ap);
  return result;
}

void stringAppendV(std::string* dst, const char* format, va_list ap) {
    StringBuilder<1024> builder;

    va_list apCopy;
    va_copy(apCopy, ap);
    int result = builder.vappend(format,apCopy);
    va_end(apCopy);
    if (result >= 0 && result < static_cast<int>(builder.capacity())) {
        dst->append(builder.c_str(), result);
        return;
    }
     
    int mem_length = builder.capacity();
    while (true) {
        if (result < 0) {
            if (errno != 0 && errno != EOVERFLOW)
            {
                ESP_LOGE(TAG, "Unable to printf the requested string due to error.");
                return;
            }
            mem_length *= 2;
        } else {
            mem_length = result + 1;
        }
        if (mem_length > 32 * 1024 * 1024) {
            ESP_LOGE(TAG, "Unable to printf the requested string due to size.");
            return;
        }
        DynamicStringBuilder dynamicBuilder(mem_length);
        va_copy(apCopy, ap);
        int result = dynamicBuilder.vappend(format,apCopy);
        va_end(apCopy);
        if ((result >= 0) && (result < mem_length)) {
            dst->append(dynamicBuilder.c_str(), result);
            return;
        }
    }
}

} // namespace Core
} // namespace Bt

