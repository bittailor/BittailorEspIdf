//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::Singleton
//
//*************************************************************************************************

#ifndef INC__Bt_Core_Singleton__h
#define INC__Bt_Core_Singleton__h

#include <stdexcept>

namespace Bt {
namespace Core {

template<typename T>
class Singleton
{
   public:
      Singleton() = delete;
      Singleton(const Singleton&) = delete;
      Singleton& operator=(const Singleton&) = delete;
      ~Singleton() = delete;

      static bool valid() {
         return mInstance != nullptr;
      }

      static T& instance() {
         if(!valid()) {
            throw std::logic_error("No singleton instance set"); 
         }
         return *mInstance;
      }

      class Instance
      {
         public:
            Instance(T& iInstance)
            : mPrevious(Singleton::mInstance)
            {
               Singleton::mInstance = &iInstance;
            }

            ~Instance()
            {
               Singleton::mInstance = mPrevious;
            }

         private:
            Instance(const Instance&);
            Instance& operator=(const Instance&);
            T* mPrevious;
      };

      friend class Singleton::Instance;

   private:
      static T* mInstance;
};

template <typename T>
T* Singleton<T>::mInstance = nullptr;

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_Singleton__h

