#include "Bt/Core/StringBuilder.h"

#include <stdio.h>
#include <stdarg.h>

#include "Bt/Core/Logging.h"

namespace Bt {
namespace Core {
namespace {
   const char* TAG = "StringBuilder";
}

StringBuilderBase& StringBuilderBase::append(const char* pFormat, ...) {
   va_list arglist;
   va_start(arglist, pFormat);
   size_t lenght = vsnprintf(mBuffer+mLenght, mSize-mLenght, pFormat,arglist);
   va_end( arglist );
   if(lenght > (mSize - mLenght - 1) ){
      ESP_LOGW(TAG, "StringBuilder full mLenght=%u mSize=%u", mLenght, mSize);
      mLenght = mSize;
      return *this;
   }
   mLenght += lenght;
   return *this;
}

StringBuilderBase& StringBuilderBase::hexencode(const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (i + 1 != len) {
      append("%02X.", data[i]);
    } else {
      append("%02X ", data[i]);
    }
  }
  return append("(%u)", len);
}

StringBuilderBase& StringBuilderBase::reset() {
   mBuffer[0] = 0;
   mLenght = 0;
   return *this;
}

} // namespace Core
} // namespace Bt
