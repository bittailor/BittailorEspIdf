//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Events::Events
//
//*************************************************************************************************

#ifndef INC__Bt_Events_Events__h
#define INC__Bt_Events_Events__h

#include <list>
#include <mutex>

#include <Bt/Core/Logging.h>
#include <Bt/Concurrency/I_ExecutionContext.h>

#include "Bt/Events/Tag.h"

namespace Bt {
namespace Events {

template <typename E> class Subscription;
template<typename E> void publish(E pEvent);

//-------------------------------------------------------------------------------------------------

template <typename E>
class EventController {
	private:
		friend class Subscription<E>;
		friend void publish<E>(E pEvent);

		static EventController<E>& getInstance()
		{
			static EventController<E> sInstance;
			return sInstance;
		}

		void add(Subscription<E>* supscription) {
			std::lock_guard<std::mutex> lock(mMutex);
			mSupscriptions.push_back(supscription);
		}

		void remove(Subscription<E>* supscription) {
			std::lock_guard<std::mutex> lock(mMutex);
			mSupscriptions.remove(supscription);
		}

		std::list<Subscription<E>*> supscriptions() {
			std::lock_guard<std::mutex> lock(mMutex);
			return mSupscriptions;
		}

		std::mutex mMutex;
		std::list<Subscription<E>*> mSupscriptions;

};

//-------------------------------------------------------------------------------------------------

template <typename E>
class Subscription {
	public:
		Subscription(Concurrency::I_ExecutionContext& pExecutionContext, std::function<void(E)> pCallback)
	    : mExecutionContext(pExecutionContext), mCallback(pCallback)  {
			EventController<E>::getInstance().add(this);
		}
		Subscription(const Subscription&) = delete;
		Subscription& operator=(const Subscription&) = delete;
		~Subscription() {
			EventController<E>::getInstance().remove(this);
		}

	private:
		friend void publish<E>(E pEvent);

		void notify(const E& pEvent){
			if(Concurrency::I_ExecutionContext::current() == &mExecutionContext) {
				mCallback(pEvent);
			} else {
				mExecutionContext.call([this,pEvent](){mCallback(pEvent);});
			}
		}

		Concurrency::I_ExecutionContext& mExecutionContext;
		std::function<void(E)> mCallback;

};

//-------------------------------------------------------------------------------------------------

template<typename E> void publish(E pEvent) {
	auto supscriptions = EventController<E>::getInstance().supscriptions();
	ESP_LOGV(TAG, "Publish %s to %d supscriptions", typeid(E).name(), supscriptions.size());
	for(Subscription<E>* supscription : supscriptions){
		supscription->notify(pEvent);
	}
}

} // namespace Events
} // namespace Bt

#endif // INC__Bt_Events_Events__h
