//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::TimeStub
//
//*************************************************************************************************

#include "Bt/Core/TimeStub.h"

#include <chrono>

namespace Bt {
namespace Core {

uint32_t TimeStub::milliseconds() {
   auto epoch = std::chrono::steady_clock::now().time_since_epoch();
   std::chrono::milliseconds epochInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
   return epochInMilliseconds.count();
}

uint32_t TimeStub::microseconds() {
   auto epoch = std::chrono::steady_clock::now().time_since_epoch();
   std::chrono::microseconds epochInMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
   return epochInMicroseconds.count();
}

std::chrono::system_clock::time_point TimeStub::now() {
   return std::chrono::system_clock::now();
}

} // namespace Core
} // namespace Bt

