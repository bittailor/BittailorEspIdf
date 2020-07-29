//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::TimeMock
//
//*************************************************************************************************

#ifndef INC__Bt_Core_TimeMock__h
#define INC__Bt_Core_TimeMock__h

#include <Bt/Core/I_Time.h>

namespace Bt {
namespace Core {

class TimeMock : public I_Time
{
   public:
      TimeMock();
      TimeMock(const TimeMock&) = delete;
      TimeMock& operator=(const TimeMock&) = delete;
      ~TimeMock();

      virtual uint32_t milliseconds() {return mMilliseconds;}
      virtual uint32_t microseconds() {return mMicroseconds;}
      virtual std::chrono::system_clock::time_point now() {return mNow;}

      uint32_t mMilliseconds;
      uint32_t mMicroseconds;
      std::chrono::system_clock::time_point mNow;
     
   private:
      
};

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_TimeMock__h

