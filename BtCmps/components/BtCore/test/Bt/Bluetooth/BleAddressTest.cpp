//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>

#include "Bt/Bluetooth/BleAddress.h"

namespace Bt {
namespace Bluetooth {

TEST_CASE( "BleAddressTest", "[BleAddressTest]" ) {
   
   SECTION("empty") {
      BleAddress mBleUuid;
      REQUIRE(mBleUuid.isEmpty());
   }  

   SECTION("from LE") {
      BleAddress mBleUuid = BleAddress::from48BitLe(BleAddress::Address48Bit{0xDB, 0x1D, 0x7B, 0x38, 0xC1, 0xA4});
      // {0xA4, 0xC1, 0x38, 0x7B, 0x1D, 0xDB}
       //{0xDB, 0x1D, 0x7B, 0x38, 0xC1, 0xA4};
      REQUIRE(!mBleUuid.isEmpty());
      REQUIRE(mBleUuid.toString() == "A4:C1:38:7B:1D:DB");
   }   
   
}

} // namespace Bluetooth
} // namespace Bt

