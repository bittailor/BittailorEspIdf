//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Peripherals::I_DigitalIn
//
//*************************************************************************************************

#ifndef INC__Bt_Peripherals_I_DigitalIn__h
#define INC__Bt_Peripherals_I_DigitalIn__h

namespace Bt {
namespace Peripherals {

class I_DigitalIn {
   public:
      virtual ~I_DigitalIn() {}

      virtual bool read() = 0;
};

} // namespace Peripherals
} // namespace Bt

#endif // INC__Bt_Peripherals_I_DigitalIn__h
