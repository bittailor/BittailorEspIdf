//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::Timezone
//
//*************************************************************************************************

#ifndef INC__Bt_Core_Timezone__h
#define INC__Bt_Core_Timezone__h

#include <tuple>
#include <date/date.h>

#include "Bt/Core/I_Timezone.h"

namespace Bt {
namespace Core {

class Timezone : public I_Timezone
{
   public:
      Timezone();
      Timezone(const Timezone&) = delete;
      Timezone& operator=(const Timezone&) = delete;
      ~Timezone();

      virtual std::chrono::system_clock::time_point toLocal(std::chrono::system_clock::time_point pUtc);
      virtual std::chrono::system_clock::time_point toUTC(std::chrono::system_clock::time_point pLocal);

      virtual TimeTuple localTime(std::chrono::system_clock::time_point pUtc);

   private:
      
};

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_Timezone__h

