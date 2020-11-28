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
   

   SECTION("empty") {
      BleUuid mBleUuid;
      REQUIRE(mBleUuid.isEmpty());
   }  

   SECTION("16Bit form raw LE") {
      uint8_t leRaw[] = {0x95,0xFE}; 
      BleUuid mBleUuid = BleUuid::from16BitLe(leRaw);
      REQUIRE(!mBleUuid.isEmpty());
      REQUIRE(mBleUuid == 0xfe95);
      REQUIRE_FALSE(mBleUuid == 0xaffe);
   } 

   SECTION("16Bit from uint16") {
      BleUuid mBleUuid = BleUuid::from16Bit(0xfe95);
      REQUIRE(!mBleUuid.isEmpty());
      REQUIRE(mBleUuid == 0xfe95);
      REQUIRE_FALSE(mBleUuid == 0xaffe);
   }

   SECTION("16Bit from string short") {
      BleUuid mBleUuid = BleUuid::fromString("fe95");
      REQUIRE(!mBleUuid.isEmpty());
      REQUIRE(mBleUuid == 0xfe95);
      REQUIRE_FALSE(mBleUuid == 0xaffe);
   }

   SECTION("16Bit from string long") {
      BleUuid mBleUuid = BleUuid::fromString("0000fe95-0000-1000-8000-00805f9b34fb");
      REQUIRE(!mBleUuid.isEmpty());
      REQUIRE(mBleUuid.is16Bit());
      REQUIRE(mBleUuid == 0xfe95);
      REQUIRE_FALSE(mBleUuid == 0xaffe);
   }

   SECTION("16Bit string compare") {
      BleUuid mBleUuid = BleUuid::from16Bit(0xfe95);
      REQUIRE(!mBleUuid.isEmpty());
      REQUIRE(mBleUuid.toString()  == "0000fe95-0000-1000-8000-00805f9b34fb");
      REQUIRE_FALSE(mBleUuid == 0xaffe);
   }

}

} // namespace Bluetooth
} // namespace Bt

