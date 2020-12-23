//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::System::System
//
//*************************************************************************************************

#ifndef INC__Bt_System_System__h
#define INC__Bt_System_System__h

#include "Bt/System/I_System.h"

namespace Bt {
namespace System {

class System : public I_System
{
   public:
      System();
      System(const System&) = delete;
      System& operator=(const System&) = delete;
      ~System();

      virtual const std::string& id() {return mId;}

   private:
      std::string mId;
      
};

} // namespace System
} // namespace Bt

#endif // INC__Bt_System_System__h

