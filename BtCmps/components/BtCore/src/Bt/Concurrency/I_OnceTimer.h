//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Concurrency::I_OnceTimer
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_I_OnceTimer__h
#define INC__Bt_Concurrency_I_OnceTimer__h

namespace Bt {
namespace Concurrency {

class I_OnceTimer {
   public:
      virtual ~I_OnceTimer(){};    
      virtual void cancel() = 0;
};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_I_OnceTimer__h

