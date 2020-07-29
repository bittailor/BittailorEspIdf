//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Concurrency::SchedulingExecutionContext
//
//*************************************************************************************************

#include "Bt/Concurrency/SchedulingExecutionContext.h"

#include <future>
#include <iostream>

namespace Bt {
namespace Concurrency {

SchedulingExecutionContext::SchedulingExecutionContext(Core::I_Time& pTime): SchedulingExecutionContextBase(pTime), mRunning(false) {

}

SchedulingExecutionContext::~SchedulingExecutionContext() {
}

void SchedulingExecutionContext::run() {
	mRunning = true;
	auto previous = setCurrentExecutionContext(this);
	while(mRunning) {
		workcycle();
	}
	setCurrentExecutionContext(previous);
}

void SchedulingExecutionContext::stop() {
	call([this](){mRunning = false;});
}

std::chrono::milliseconds SchedulingExecutionContext::maxWorkcycleTimeout() {
	return mRunning ? std::chrono::milliseconds(10000) : std::chrono::milliseconds(0) ;
}

} // namespace Concurrency
} // namespace Bt
