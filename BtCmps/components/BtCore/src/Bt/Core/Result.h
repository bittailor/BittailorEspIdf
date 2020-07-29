//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Core::Result
//
//*************************************************************************************************

#ifndef INC__Bt_Core_Result__h
#define INC__Bt_Core_Result__h

#include <utility>

namespace Bt {
namespace Core {

template<typename T>
class Result
{
   public:
      Result():mValid{false}{

      }

      Result(const T& pValue):mValid(true), mValue(pValue){
      }

      Result(const std::pair<bool,T>& pResult):mValid(pResult.first), mValue(pResult.second){
      }

      explicit operator bool() const {
         return mValid;
      }

      const T& value() const {
         return mValue;
      }

   private:
      bool mValid;
      T mValue;
};

} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_Result__h
