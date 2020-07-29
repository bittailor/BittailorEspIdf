#ifndef INC__Bt_Core_StringBuilder__h
#define INC__Bt_Core_StringBuilder__h

#include <array>

namespace Bt {
namespace Core {

class StringBuilderBase {
   protected:
      StringBuilderBase(char* pBuffer, size_t pSize)
      : mBuffer{pBuffer}, mSize{pSize}, mLenght{0}{
      }
   public:
      int append(const char* pFormat, ...) __attribute__ ((format (printf, 2, 3)));


      const char* c_str() {
         return mBuffer;
      }

   private:
      char* mBuffer;
      size_t mSize;
      size_t mLenght;

};

template<size_t N>
class StringBuilder : public StringBuilderBase  {
   public:
      StringBuilder(): StringBuilderBase{mBuffer.data(),N}, mBuffer{0}{
      }
   private:
      std::array<char,N> mBuffer;
};


} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_StringBuilder__h
