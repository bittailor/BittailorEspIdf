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
      REQUIRE(!mBleUuid.isEmpty());
      REQUIRE(mBleUuid.toString() == "A4:C1:38:7B:1D:DB");
   }   

   SECTION("from LE") {
      BleAddress mBleUuid = BleAddress::from48BitBe(BleAddress::Address48Bit{0xA4, 0xC1, 0x38, 0x7B, 0x1D, 0xDB});
      REQUIRE(!mBleUuid.isEmpty());
      REQUIRE(mBleUuid.toString() == "A4:C1:38:7B:1D:DB");
   } 

   SECTION("compare") {
      BleAddress one = BleAddress::from48BitLe(BleAddress::Address48Bit{0xDB, 0x1D, 0x7B, 0x38, 0xC1, 0xA4});
      BleAddress two = BleAddress::from48BitBe(BleAddress::Address48Bit{0xA4, 0xC1, 0x38, 0x7B, 0x1D, 0xDB});
      REQUIRE(one == two);
   } 

   SECTION("compare") {
      BleAddress one = BleAddress::from48BitLe(BleAddress::Address48Bit{0xDB, 0x1D, 0x7B, 0x38, 0xC1, 0xA4});
      BleAddress two = BleAddress::from48BitLe(BleAddress::Address48Bit{0x67, 0x1D, 0x7B, 0x38, 0xC1, 0xA4});
      REQUIRE(one != two);
   }   
   
}

} // namespace Bluetooth
} // namespace Bt

