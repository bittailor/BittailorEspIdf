#ifndef INC__Bt_Storage_NvsRepository__h
#define INC__Bt_Storage_NvsRepository__h

#include <nvs_flash.h>

#include <Bt/Storage/I_NvsRepository.h>

namespace Bt {
namespace Storage {

class NvsRepository : public I_NvsRepository
{
   public:
      NvsRepository();
      NvsRepository(const NvsRepository&) = delete;
      NvsRepository& operator=(const NvsRepository&) = delete;
      virtual ~NvsRepository();

      virtual void commit();

      virtual bool set(const char* pKey, uint32_t pValue);
      virtual bool tryGet(const char* pKey, uint32_t& pValue);

   private:
      nvs_handle mHandle;
      
};

} // namespace Storage
} // namespace Bt

#endif // INC__Bt_Storage_NvsRepository__h
