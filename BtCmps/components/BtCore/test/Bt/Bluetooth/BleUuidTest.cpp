//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>

#include "Bt/Bluetooth/BleUuid.h"

namespace Bt {
namespace Bluetooth {

TEST_CASE( "BleUuidTest", "[BleUuidTest]" ) {
   

   SECTION("invalid") {
      BleUuid mBleUuid;
      REQUIRE_FALSE(mBleUuid.valid());
   }  

   SECTION("uint16 raw") {
      BleUuid mBleUuid({0xFE,0x95});
      REQUIRE(mBleUuid.valid());
      REQUIRE(mBleUuid == 0xfe95);
      REQUIRE_FALSE(mBleUuid == 0xaffe);
   }    
}

} // namespace Bluetooth
} // namespace Bt

