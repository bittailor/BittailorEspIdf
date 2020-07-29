//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Core::Logging
//
//*************************************************************************************************

#ifndef INC__Bt_Core_Logging__h
#define INC__Bt_Core_Logging__h

#ifdef ESP32

#else 

#include <stddef.h>

const char* pathToFileName(const char* pPath)
{
    size_t i = 0;
    size_t pos = 0;
    char * p = (char *)pPath;
    while(*p){
        i++;
        if(*p == '/' || *p == '\\'){
            pos = i;
        }
        p++;
    }
    return pPath+pos;
}

#endif

#endif // INC__Bt_Core_Logging__h

