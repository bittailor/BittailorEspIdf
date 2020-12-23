//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::System::I_System
//
//*************************************************************************************************

#ifndef INC__Bt_System_I_System__h
#define INC__Bt_System_I_System__h

#include <string>

#include <Bt/Core/Singleton.h>

namespace Bt {
namespace System {

class I_System {
   public:
      virtual ~I_System() {}

      virtual const std::string& id() =0 ;
};

inline const std::string& getId() {
   return Bt::Core::Singleton<I_System>::instance().id(); 
}      

} // namespace System
} // namespace Bt

#endif // INC__Bt_System_I_System__h

