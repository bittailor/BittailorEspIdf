//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Concurrency::CountingLatchTest
//  
//*************************************************************************************************

#include <catch.hpp>

#include <future>

#include "Bt/Concurrency/CountingLatch.h"

namespace  Bt {
namespace Concurrency {
namespace {

   bool WaitLatch(CountingLatch& iLatch) {
      iLatch.wait();
      return true;
   }

   bool WaitForLatch(CountingLatch& iLatch, std::chrono::milliseconds iTimeout) {
      return iLatch.waitFor(iTimeout);
   }

}

TEST_CASE("CountingLatchTest::TwoThreadsWaitLatchOfOne", "[CountingLatchTest][Concurrency]") {
   CountingLatch latch(0);
   latch.countUp();

   auto futureOne = std::async(std::launch::async, WaitLatch, std::ref(latch));
   auto futureTwo = std::async(std::launch::async, WaitLatch, std::ref(latch));

   latch.countDown();

   REQUIRE(std::future_status::ready == futureOne.wait_for(std::chrono::seconds(1)));
   REQUIRE(std::future_status::ready == futureTwo.wait_for(std::chrono::seconds(1)));

}

TEST_CASE("CountingLatchTest::TwoThreadsWaitLatchAlreadyZero", "[CountingLatchTest][Concurrency]") {
   CountingLatch latch(0);
   latch.countUp();
   latch.countDown();

   auto futureOne = std::async(std::launch::async, WaitLatch, std::ref(latch));
   auto futureTwo = std::async(std::launch::async, WaitLatch, std::ref(latch));

   REQUIRE(std::future_status::ready == futureOne.wait_for(std::chrono::seconds(1)));
   REQUIRE(std::future_status::ready == futureTwo.wait_for(std::chrono::seconds(1)));

}

TEST_CASE("CountingLatchTest::OneThreadsWaitLatchOfTwo", "[CountingLatchTest][Concurrency]") {
   CountingLatch latch(0);
   latch.countUp();
   latch.countUp();

   auto futureOne = std::async(std::launch::async, WaitLatch, std::ref(latch));

   REQUIRE(std::future_status::timeout == futureOne.wait_for(std::chrono::milliseconds(100)));

   latch.countDown();

   REQUIRE(std::future_status::timeout == futureOne.wait_for(std::chrono::milliseconds(100)));

   latch.countDown();

   REQUIRE(std::future_status::ready == futureOne.wait_for(std::chrono::seconds(1)));
}

TEST_CASE("CountingLatchTest::OneThreadsWaitForLatchOfTwo", "[CountingLatchTest][Concurrency]") {
   CountingLatch latch(0);
   latch.countUp();
   latch.countUp();

   auto futureFirst = std::async(std::launch::async, WaitForLatch, std::ref(latch), std::chrono::milliseconds(100));
   REQUIRE_FALSE(futureFirst.get());

   auto futureSecond = std::async(std::launch::async, WaitForLatch, std::ref(latch), std::chrono::milliseconds(100));
   latch.countDown();
   REQUIRE_FALSE(futureSecond.get());

   auto futureThird = std::async(std::launch::async, WaitForLatch, std::ref(latch), std::chrono::milliseconds(100));
   latch.countDown();
   REQUIRE(futureThird.get());
}


} // namespace Concurrency
} // namespace Bt
