//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Peripherals::I_DigitalOut
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_I_DigitalOut__h
#define INC__Bt_Peripherals_I_DigitalOut__h

namespace Bt {
namespace Peripherals {

class I_DigitalOut {
   public:
      virtual ~I_DigitalOut() {}
      
      virtual void write(bool pValue) = 0;
      virtual bool value() = 0;
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_I_DigitalOut__h

