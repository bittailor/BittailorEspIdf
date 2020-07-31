#include "Bt/Core/StringBuilder.h"

#include <stdio.h>
#include <stdarg.h>

#include "Bt/Core/Logging.h"

namespace Bt {
namespace Core {
namespace {
   const char* TAG = "StringBuilder";
}

int StringBuilderBase::append(const char* pFormat, ...) {
   va_list arglist;
   va_start(arglist, pFormat);
   size_t lenght = vsnprintf(mBuffer+mLenght, mSize-mLenght, pFormat,arglist);
   va_end( arglist );
   if(lenght > (mSize - mLenght - 1) ){
      ESP_LOGW(TAG, "StringBuilder full mLenght=%u mSize=%u", mLenght, mSize);
      mLenght = mSize;
      return mLenght;
   }
   mLenght += lenght;
   return mLenght;
}

} // namespace Core
} // namespace Bt
