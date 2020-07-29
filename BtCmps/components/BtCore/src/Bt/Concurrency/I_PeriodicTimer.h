//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Concurrency::I_PeriodicTimer
//
//*************************************************************************************************

#ifndef INC__Bt_Concurrency_I_PeriodicTimer__h
#define INC__Bt_Concurrency_I_PeriodicTimer__h

namespace Bt {
namespace Concurrency {

class I_PeriodicTimer {
   public:
      virtual ~I_PeriodicTimer(){};    
      virtual void cancel() = 0;
};

} // namespace Concurrency
} // namespace Bt

#endif // INC__Bt_Concurrency_I_PeriodicTimer__h

