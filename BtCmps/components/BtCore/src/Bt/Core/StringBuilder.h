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
      StringBuilderBase& append(const char* pFormat, ...) __attribute__ ((format (printf, 2, 3)));
      int vappend(const char* pFormat, va_list pArglist);
      
      StringBuilderBase& hexencode(const uint8_t *data, size_t len);
      template<typename T>
      StringBuilderBase& hexencode(const T& container) {return hexencode(container.data(),container.size());}
      
      StringBuilderBase& reset();

      const char* c_str() {
         return mBuffer;
      }

      size_t capacity() const {return mSize;}

   private:
      char* mBuffer;
      size_t mSize;
      size_t mLenght;

};

template<size_t N>
class StringBuilder : private std::array<char,N>,  public StringBuilderBase  {
   public:
      StringBuilder(): StringBuilderBase{this->data(),N} {
      }
   private:
};

typedef StringBuilder<200> DefaultStringBuilder; 

class DynamicStringBuilder : public StringBuilderBase  {
   public:
      DynamicStringBuilder(size_t pCapacity): StringBuilderBase{allocate(pCapacity), pCapacity}, mBuffer{0}{
      }
      ~DynamicStringBuilder(){
         delete[] mBuffer; 
      }
   private:
      char* allocate(size_t pCapacity) {
         mBuffer = new char[pCapacity];
         return mBuffer;  
      }

      char* mBuffer;
};


} // namespace Core
} // namespace Bt

#endif // INC__Bt_Core_StringBuilder__h
