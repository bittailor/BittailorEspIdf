//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_ExecutionContextMock__h
#define INC__Bt_Concurrency_ExecutionContextMock__h

/*
inline std::ostream& operator<<(std::ostream& pOut, const std::chrono::milliseconds& pDuration) {
   pOut << pDuration.count() << "ms";
   return pOut;
}
*/

#include <trompeloeil.hpp>

#include "Bt/Concurrency/I_ExecutionContext.h"

namespace Bt {
namespace Concurrency {

class ExecutionContextMock : public I_ExecutionContext {
   public:

      MAKE_MOCK1(call, void(std::function<void()>),override);
};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_ExecutionContextMock__h

