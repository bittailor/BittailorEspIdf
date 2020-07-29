//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Concurrency::BlockingQueueTest
//  
//*************************************************************************************************

#include <chrono>
#include <catch.hpp>

#include "Bt/Concurrency/BlockingQueue.h"


namespace Bt {
namespace Concurrency {

TEST_CASE( "BlockingQueueTest", "[BlockingQueueTest][Concurrency]" ) {

   BlockingQueue<int> mBlockingQueue;

   SECTION("emptyOnEmptyReturnsTrue") {
      REQUIRE(mBlockingQueue.empty());
   }

   SECTION("emptyOnNonEmptyReturnsFalse") {
      mBlockingQueue.push(1977);
      REQUIRE_FALSE(mBlockingQueue.empty());
   }

   SECTION("tryPopOnEmptyReturnFalse") {
      int value;
      REQUIRE_FALSE(mBlockingQueue.tryPop(value));
   }

   SECTION("tryPopOnNonEmptyReturnTrue") {
      mBlockingQueue.push(1977);
      int value;
      REQUIRE(mBlockingQueue.tryPop(value));
   }

   SECTION("tryPopOnNonEmptySetsValue") {
      mBlockingQueue.push(1977);
      int value = 0;
      mBlockingQueue.tryPop(value);
      REQUIRE(1977 == value);
   }

   SECTION("popOnNonEmptySetsValue") {
      mBlockingQueue.push(1977);
      int value = 0;
      mBlockingQueue.pop(value);
      REQUIRE(1977 == value);
   }

   SECTION("pushPopOrder") {
      mBlockingQueue.push(1977);
      mBlockingQueue.push(2000);
      mBlockingQueue.push(2014);
      int value = 0;
      mBlockingQueue.pop(value);
      REQUIRE(1977 == value);
      mBlockingQueue.pop(value);
      REQUIRE(2000 == value);
      mBlockingQueue.pop(value);
      REQUIRE(2014 == value);
   }

   SECTION("tryPopWithTimeoutOnEmptyReturnsFalse") {
      int value = 0;
      REQUIRE_FALSE(mBlockingQueue.tryPop(value, std::chrono::milliseconds(1)));
   }

   SECTION("tryPopWithTimeoutOnEmptyWaits") {
      int value = 0;
      auto start = std::chrono::system_clock::now();
      mBlockingQueue.tryPop(value, std::chrono::milliseconds(300));
      std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
      REQUIRE(duration >= std::chrono::milliseconds(300));
   }

   SECTION("tryPopWithTimeoutOnNonEmptyDoesNotWait") {
      mBlockingQueue.push(1977);
      int value = 0;
      auto start = std::chrono::system_clock::now();
      mBlockingQueue.tryPop(value, std::chrono::seconds(10));
      std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
      REQUIRE(duration < std::chrono::milliseconds(10));
   }

   SECTION("waitForWithTimeoutOnEmptyReturnsFalse") {
      REQUIRE_FALSE(mBlockingQueue.waitFor(std::chrono::milliseconds(1)));
   }

   SECTION("waitForWithTimeoutOnEmptyWaits") {
      auto start = std::chrono::system_clock::now();
      mBlockingQueue.waitFor(std::chrono::milliseconds(300));
      std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
      REQUIRE(duration >= std::chrono::milliseconds(300));
   }

   SECTION("waitForWithTimeoutOnNonEmptyDoesNotWait") {
      mBlockingQueue.push(1977);
      auto start = std::chrono::system_clock::now();
      mBlockingQueue.waitFor(std::chrono::seconds(10));
      std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
      REQUIRE(duration < std::chrono::milliseconds(10));
   }

   SECTION("waitOnNonEmptyDoesNotWait") {
      mBlockingQueue.push(1977);
      auto start = std::chrono::system_clock::now();
      mBlockingQueue.wait();
      std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
      REQUIRE(duration < std::chrono::milliseconds(10));
   }

   SECTION("moveOnlyType") {
      BlockingQueue<std::unique_ptr<int>> queue;

      std::unique_ptr<int> data1(new int(1));

      queue.push(std::move(data1));
      queue.push(std::unique_ptr<int>(new int(2)));

      std::unique_ptr<int> out1;
      std::unique_ptr<int> out2;
      std::unique_ptr<int> out3;
      queue.peek();
      queue.pop(out1);
      queue.tryPop(out2);
      queue.tryPop(out3);
   }
}

} // namespace Concurrency
} // namespace Bt
