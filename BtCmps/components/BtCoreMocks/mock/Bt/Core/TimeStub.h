//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::TimeStub
//
//*************************************************************************************************

#ifndef INC__Bt_Core_TimeStub__h
#define INC__Bt_Core_TimeStub__h

#include <Bt/Core/I_Time.h>

namespace Bt {
namespace Core {

class TimeStub : public I_Time
{
   public:

      virtual uint32_t milliseconds();
      virtual uint32_t microseconds();
      virtual std::chrono::system_clock::time_point now();
      
};

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_TimeStub__h

