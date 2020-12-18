//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::I_ExecutionContext
//
//*************************************************************************************************

#include "Bt/Concurrency/I_ExecutionContext.h"

namespace Bt {
namespace Concurrency {
namespace {
   thread_local I_ExecutionContext* tExecutionContext = nullptr;
}

I_ExecutionContext* I_ExecutionContext::current() {
    return tExecutionContext;
}

I_ExecutionContext* I_ExecutionContext::setCurrentExecutionContext(I_ExecutionContext* pCurrent) {
    I_ExecutionContext* previous = tExecutionContext;
    tExecutionContext = pCurrent;
    return previous;    
}

void I_ExecutionContext::ensureCallOnContext(std::function<void()> pFunction) {
    if(current() == this) {
        pFunction();    
    } else {
        call(pFunction);     
    }
}

} // namespace Concurrency
} // namespace Bt
