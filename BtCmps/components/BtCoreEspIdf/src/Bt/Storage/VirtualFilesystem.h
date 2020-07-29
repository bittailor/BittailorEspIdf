//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Storage::VirtualFilesystem
//
//*************************************************************************************************

#ifndef INC__Bt_Storage_VirtualFilesystem__h
#define INC__Bt_Storage_VirtualFilesystem__h

#include <cstddef>

namespace Bt {
namespace Storage {

class VirtualFilesystem
{
   public:
      VirtualFilesystem();
      VirtualFilesystem(const VirtualFilesystem&) = delete;
      VirtualFilesystem& operator=(const VirtualFilesystem&) = delete;
      ~VirtualFilesystem();

      void registerSpiffsPartition(const char* pBasePath, const char* pPartitionLabel, bool formatIfMountFailed = false, size_t pMaxFiles = 10);

   private:
      
};

} // namespace Storage
} // namespace Bt

#endif // INC__Bt_Storage_VirtualFilesystem__h
