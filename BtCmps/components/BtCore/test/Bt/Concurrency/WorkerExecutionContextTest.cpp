//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>

#include <set>
#include <future>

#include "Bt/Concurrency/WorkerExecutionContext.h"
#include "Bt/Concurrency/CountdownLatch.h"

using namespace std::literals::chrono_literals;

namespace Bt {
namespace Concurrency {
namespace {
   thread_local int tTestId = -1;
}   

TEST_CASE( "WorkerExecutionContextTest", "[WorkerExecutionContextTest]" ) {
   
   SECTION("stop after first call ensure all calls are executed and run exists") {
      std::vector<int> callIds;     
      {
         CountdownLatch latch(1);
         WorkerExecutionContext workerExecutionContext;


         workerExecutionContext.call([&callIds,&latch](){
            callIds.push_back(1);
            latch.countDown();
         });
         workerExecutionContext.call([&callIds](){
            callIds.push_back(2);
         });
         workerExecutionContext.call([&callIds](){
            callIds.push_back(3);
         });
         
         std::thread thread([&workerExecutionContext](){workerExecutionContext.run();});

         latch.wait();
         workerExecutionContext.stop();
         thread.join();         

      }
      REQUIRE_THAT(callIds, Catch::Equals(std::vector<int>{1,2,3}) );
   }   

   SECTION("stop even before run() ensure all calls are executed and run exists") {
      std::vector<int> callIds;     
      {
         CountdownLatch latch(1);
         WorkerExecutionContext workerExecutionContext;

         workerExecutionContext.call([&callIds](){
            callIds.push_back(1);
         });
         workerExecutionContext.call([&callIds](){
            callIds.push_back(2);
         });
         workerExecutionContext.call([&callIds](){
            callIds.push_back(3);
         });
         
         std::thread thread([&workerExecutionContext, &latch](){
            latch.wait();
            workerExecutionContext.run();
         });
         
         workerExecutionContext.stop();      
         latch.countDown();
         thread.join();         

      }

      REQUIRE_THAT(callIds, Catch::Equals(std::vector<int>{1,2,3}) );
   } 

   SECTION("use multiple threads ensure all calls are distributed") {
      std::mutex mutex;
      std::vector<int> callIds; 

      {
         WorkerExecutionContext workerExecutionContext;

         std::thread threadOne([&workerExecutionContext](){
            tTestId = 1;
            workerExecutionContext.run(); 
         });
         std::thread threadTwo([&workerExecutionContext](){
            tTestId = 2;
            workerExecutionContext.run();
         });
         std::thread threadThree([&workerExecutionContext](){
            tTestId = 3;
            workerExecutionContext.run();
         });

         for (int i=0; i<10 ; i++) {
            workerExecutionContext.call([&callIds,&mutex](){
               {
                  std::unique_lock<std::mutex> lock(mutex);
                  callIds.push_back(tTestId);
               }
               std::this_thread::sleep_for(100ms);
            });
         }

         std::this_thread::sleep_for(200ms);

         workerExecutionContext.stop();      
         threadOne.join();         
         threadTwo.join();         
         threadThree.join();         
      }
      REQUIRE_THAT(callIds, Catch::VectorContains(1));
      REQUIRE_THAT(callIds, Catch::VectorContains(2));
      REQUIRE_THAT(callIds, Catch::VectorContains(3));
   }   
   
}

} // namespace Concurrency
} // namespace Bt

