//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::Time
//
//*************************************************************************************************

#ifndef INC__Bt_Core_Time__h
#define INC__Bt_Core_Time__h

#include <Bt/Core/I_Time.h>

namespace Bt {
namespace Core {

class Time : public I_Time
{
   public:
      Time();
      Time(const Time&) = delete;
      Time& operator=(const Time&) = delete;
      ~Time();

      virtual uint32_t milliseconds();
      virtual uint32_t microseconds();
      virtual TimePoint now();

   private:
      
};

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_Time__h
