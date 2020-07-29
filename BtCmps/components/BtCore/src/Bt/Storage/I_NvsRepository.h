#ifndef INC__Bt_Storage_I_NvsRepository__h
#define INC__Bt_Storage_I_NvsRepository__h

#include <cstdint>

namespace Bt {
namespace Storage {

class I_NvsRepository {
   public:
      virtual ~I_NvsRepository() {}

      virtual void commit()=0;

      virtual bool set(const char* pKey, uint32_t pValue)=0;
      virtual bool tryGet(const char* pKey, uint32_t& pValue)=0;
};

} // namespace Storage
} // namespace Bt

#endif // INC__Bt_Storage_I_NvsRepository__h
