//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::LoopExecutionContext
//
//*************************************************************************************************

#include "Bt/Concurrency/LoopExecutionContext.h"

#include <future>
#include <iostream>

namespace Bt {
namespace Concurrency {

LoopExecutionContext::LoopExecutionContext(Core::I_Time& pTime, std::chrono::milliseconds pMaxLoopTimeout)
: SchedulingExecutionContextBase(pTime), mMaxLoopTimeout(pMaxLoopTimeout)  {
}

LoopExecutionContext::~LoopExecutionContext() {
}

void LoopExecutionContext::loop() {
    auto previous = setCurrentExecutionContext(this);
    workcycle();
    setCurrentExecutionContext(previous);
}

std::chrono::milliseconds LoopExecutionContext::maxWorkcycleTimeout() {
    return mMaxLoopTimeout;
}


} // namespace Concurrency
} // namespace Bt
