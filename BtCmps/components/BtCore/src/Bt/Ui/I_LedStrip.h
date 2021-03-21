//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Ui::I_LedStrip
//
//*************************************************************************************************

#ifndef INC__Bt_Ui_I_LedStrip__h
#define INC__Bt_Ui_I_LedStrip__h

#include <cstddef>

#include <Bt/Ui/Color.h>

namespace Bt {
namespace Ui {

class I_LedStrip {
   public:
      virtual ~I_LedStrip() {}

      virtual void setPixel(size_t pIndex, const Color& pColor) = 0;
      virtual void clear() = 0;
      virtual void refresh() = 0;
      virtual size_t numberOfLeds() const = 0;
};

} // namespace Ui
} // namespace Bt

#endif // INC__Bt_Ui_I_LedStrip__h

