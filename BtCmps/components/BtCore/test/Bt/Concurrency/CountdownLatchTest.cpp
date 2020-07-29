//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Concurrency::CountdownLatchTest
//  
//*************************************************************************************************

#include <catch.hpp>

#include <future>

#include "Bt/Concurrency/CountdownLatch.h"

namespace  Bt {
namespace Concurrency {
namespace {

   bool WaitLatch(CountdownLatch& iLatch) {
      iLatch.wait();
      return true;
   }

   bool WaitForLatch(CountdownLatch& iLatch, std::chrono::milliseconds iTimeout) {
      return iLatch.waitFor(iTimeout);
   }

}

TEST_CASE("CountdownLatchTest::TwoThreadsWaitLatchOfOne", "[CountdownLatchTest][Concurrency]") {
   CountdownLatch latch(1);
   auto futureOne = std::async(std::launch::async, WaitLatch, std::ref(latch));
   auto futureTwo = std::async(std::launch::async, WaitLatch, std::ref(latch));

   latch.countDown();

   REQUIRE(std::future_status::ready == futureOne.wait_for(std::chrono::seconds(1)));
   REQUIRE(std::future_status::ready == futureTwo.wait_for(std::chrono::seconds(1)));

}

TEST_CASE("CountdownLatchTest::TwoThreadsWaitLatchAlreadyZero", "[CountdownLatchTest][Concurrency]") {
   CountdownLatch latch(1);
   latch.countDown();

   auto futureOne = std::async(std::launch::async, WaitLatch, std::ref(latch));
   auto futureTwo = std::async(std::launch::async, WaitLatch, std::ref(latch));

   REQUIRE(std::future_status::ready == futureOne.wait_for(std::chrono::seconds(1)));
   REQUIRE(std::future_status::ready == futureTwo.wait_for(std::chrono::seconds(1)));

}

TEST_CASE("CountdownLatchTest::OneThreadsWaitLatchOfTwo", "[CountdownLatchTest][Concurrency]") {
   CountdownLatch latch(2);
   auto futureOne = std::async(std::launch::async, WaitLatch, std::ref(latch));

   REQUIRE(std::future_status::timeout == futureOne.wait_for(std::chrono::milliseconds(100)));

   latch.countDown();

   REQUIRE(std::future_status::timeout == futureOne.wait_for(std::chrono::milliseconds(100)));

   latch.countDown();

   REQUIRE(std::future_status::ready == futureOne.wait_for(std::chrono::seconds(1)));
}

TEST_CASE("CountdownLatchTest::OneThreadsWaitForLatchOfTwo", "[CountdownLatchTest][Concurrency]") {
   CountdownLatch latch(2);

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
