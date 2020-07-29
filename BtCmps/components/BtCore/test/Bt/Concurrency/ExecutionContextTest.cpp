//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Concurrency::ExecutionContextTest
//  
//*************************************************************************************************

#include <catch.hpp>

#include <future>
#include <stdexcept>


#include "Bt/Core/TimeStub.h"
#include "Bt/Concurrency/SchedulingExecutionContext.h"


namespace Bt {
namespace Concurrency {namespace {
   thread_local uint32_t tId = 0;
}

TEST_CASE("ExecutionContextTest::startStop", "[ExecutionContextTest][Concurrency]") {
   auto future = std::async(std::launch::async, [](){
      Core::TimeStub timeStub;
      SchedulingExecutionContext executionContext(timeStub);
      executionContext.stop();
      executionContext.run();
      return true;
   });

   std::future_status status = future.wait_for(std::chrono::seconds(5));
   REQUIRE(std::future_status::ready == status);
}


TEST_CASE("ExecutionContextTest::checkExecutionOnThisThread", "[ExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   SchedulingExecutionContext executionContext(timeStub);
   tId = 9876;

   auto future = executionContext.callTask([](){
      return tId;
   });
   executionContext.stop();
   executionContext.run();

   REQUIRE(9876 == future.get());
}

TEST_CASE("ExecutionContextTest::checkExecutionOnWorkerThread", "[ExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   SchedulingExecutionContext executionContext(timeStub);
   tId = 9876;

   auto future = executionContext.callTask([](){
      return tId;
   });
   
   std::thread worker([&executionContext](){
      tId = 1234;
      executionContext.run();
   });

   executionContext.stop();
   worker.join();
   auto futureId = future.get();
   REQUIRE(1234 == futureId);
}


TEST_CASE("ExecutionContextTest::checkExecutionBothOnThisThread", "[ExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   SchedulingExecutionContext executionContext(timeStub);
   tId = 9876;

   auto future1 = executionContext.callTask([](){
      return tId;
   });

   auto future2 = executionContext.callTask([](){
	   return tId;
   });
   executionContext.stop();
   executionContext.run();

   REQUIRE(future1.get() == future2.get());
}


/*
TEST_CASE("ExecutionContextTest::checkException", "[ExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   SchedulingExecutionContext executionContext(timeStub);

   auto future = executionContext.callTask([](){
	   throw std::invalid_argument("just a test exception");
	   return true;
   });
   
   executionContext.stop();
   executionContext.run();

   REQUIRE_THROWS_AS(future.get(), std::invalid_argument);
}
*/


} // namespace Concurrency
} // namespace Bt
