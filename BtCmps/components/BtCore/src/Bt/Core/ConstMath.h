//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Core::ConstMath
//
//*************************************************************************************************

#ifndef INC__Bt_Core_ConstMath__h
#define INC__Bt_Core_ConstMath__h

#include <type_traits>

namespace Bt {
namespace Core {
namespace ConstMath {

template<typename T>
constexpr T pow(T x, uint32_t y) {
   return y == 0 ? 1 : x * pow(x, y-1);
}

} // namespace
} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_ConstMath__h

