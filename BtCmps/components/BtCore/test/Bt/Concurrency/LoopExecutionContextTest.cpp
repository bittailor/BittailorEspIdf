//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Concurrency::LoopExecutionContextTest
//  
//*************************************************************************************************

#include <catch.hpp>

#include <atomic>
#include <future>
#include <stdexcept>


#include "Bt/Core/TimeStub.h"
#include "Bt/Concurrency/LoopExecutionContext.h"


namespace Bt {
namespace Concurrency {namespace {
   thread_local uint32_t tId = 0;
}

TEST_CASE("LoopExecutionContextTest::startStop", "[LoopExecutionContextTest][Concurrency]") {
   auto future = std::async(std::launch::async, [](){
      Core::TimeStub timeStub;
      LoopExecutionContext executionContext(timeStub, std::chrono::milliseconds(1));
      executionContext.loop();
      return true;
   });

   std::future_status status = future.wait_for(std::chrono::seconds(5));
   REQUIRE(std::future_status::ready == status);
}


TEST_CASE("LoopExecutionContextTest::checkExecutionOnThisThread", "[LoopExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   LoopExecutionContext executionContext(timeStub, std::chrono::milliseconds(1));
   tId = 9876;

   auto future = executionContext.callTask([](){
      return tId;
   });
   executionContext.loop();

   REQUIRE(9876 == future.get());
}

TEST_CASE("LoopExecutionContextTest::checkExecutionOnWorkerThread", "[LoopExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   LoopExecutionContext executionContext(timeStub, std::chrono::milliseconds(1));
   tId = 9876;

   auto future = executionContext.callTask([](){
      return tId;
   });
   
   std::thread worker([&executionContext](){
      tId = 1234;
      executionContext.loop();
   });

   worker.join();
   auto futureId = future.get();
   REQUIRE(1234 == futureId);
}


TEST_CASE("LoopExecutionContextTest::checkExecutionBothOnThisThread", "[LoopExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   LoopExecutionContext executionContext(timeStub, std::chrono::milliseconds(1));
   tId = 9876;

   auto future1 = executionContext.callTask([](){
      return tId;
   });

   auto future2 = executionContext.callTask([](){
	   return tId;
   });

   executionContext.loop();

   REQUIRE(future1.get() == future2.get());
}

TEST_CASE("LoopExecutionContextTest::ensureCallOnContextCallFromOtherContext", "[LoopExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   LoopExecutionContext executionContext(timeStub, std::chrono::milliseconds(1));
   std::atomic<bool> executed = false;
   std::atomic<I_ExecutionContext*> executedOn = nullptr;

   
   executionContext.ensureCallOnContext([&executed,&executedOn](){
      executed = true;
      executedOn = I_ExecutionContext::current();
   });

   REQUIRE(executed == false);
 
   executionContext.loop();

   REQUIRE(executed == true);
   REQUIRE(executedOn == &executionContext);
}

TEST_CASE("LoopExecutionContextTest::ensureCallOnContextCallFromSameContext", "[LoopExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   LoopExecutionContext executionContext(timeStub, std::chrono::milliseconds(1));
   std::atomic<bool> executed = false;
   std::atomic<I_ExecutionContext*> executedOn = nullptr;

   
   executionContext.ensureCallOnContext([&executed,&executedOn,&executionContext](){
      executionContext.ensureCallOnContext([&executed,&executedOn](){
         executed = true;
         executedOn = I_ExecutionContext::current();
      });
   });

   REQUIRE(executed == false);
 
   executionContext.loop();

   REQUIRE(executed == true);
   REQUIRE(executedOn == &executionContext);
}

/*
TEST_CASE("LoopExecutionContextTest::checkException", "[LoopExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   LoopExecutionContext executionContext(timeStub, std::chrono::milliseconds(1));

   auto future = executionContext.callTask([](){
	   throw std::invalid_argument("just a test exception");
	   return true;
   });
   
   executionContext.loop();
   REQUIRE_THROWS_AS(future.get(), std::invalid_argument);
 
}


TEST_CASE("LoopExecutionContextTest::checkDomainErrorException", "[LoopExecutionContextTest][Concurrency]") {
   Core::TimeStub timeStub;
   LoopExecutionContext executionContext(timeStub, std::chrono::milliseconds(1));

   auto future = executionContext.callTask([](){
	   throw std::domain_error("just a domain_error exception");
	   return true;
   });
   
   executionContext.loop();
   REQUIRE_THROWS_AS(future.get(), std::domain_error);
 
}
*/



} // namespace Concurrency
} // namespace Bt
