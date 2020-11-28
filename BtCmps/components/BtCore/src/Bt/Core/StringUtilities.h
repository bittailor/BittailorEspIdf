//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::StringUtilities
//
//*************************************************************************************************

#ifndef INC__Bt_Core_StringUtilities__h
#define INC__Bt_Core_StringUtilities__h

#include <string>

namespace Bt {
namespace Core {

std::string stringPrintf(const char* format, ...) __attribute__ ((format (printf, 1, 2)));

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_StringUtilities__h

