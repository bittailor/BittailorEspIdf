//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>

#include <vector>

#include "Bt/Core/TimeMock.h"
#include "Bt/Concurrency/SchedulingExecutionContextBase.h"

using namespace std::chrono;

namespace Bt {
namespace Concurrency {

class 
TestingSchedulingExecutionContext : public SchedulingExecutionContextBase {
   public:
      TestingSchedulingExecutionContext(Core::I_Time& pTime): SchedulingExecutionContextBase(pTime){}
      void doWorkcycle(){workcycle();}
      uint32_t mMaxWorkcycleTimeout = 0;
   protected:
      virtual milliseconds maxWorkcycleTimeout() {return milliseconds(mMaxWorkcycleTimeout);}

};

TEST_CASE( "SchedulingExecutionContextBaseTest", "[SchedulingExecutionContextBaseTest]") {
   Core::TimeMock time;
   TestingSchedulingExecutionContext executionContext(time);
   
   auto countUpTo = [&time, &executionContext](uint32_t pMilliseconds, uint32_t steps = 1){
      while(time.milliseconds() < pMilliseconds) {
         time.mMilliseconds += steps;
         executionContext.doWorkcycle();
      }
   };

   SECTION("one once timer executes only once") {
      std::vector<int> callsAtMillis;
      time.mMilliseconds = 1000;

      executionContext.callOnce(milliseconds(10),[&callsAtMillis,&time]{
         callsAtMillis.push_back(time.milliseconds());
      });

      countUpTo(2000);
   
      REQUIRE_THAT(callsAtMillis, Catch::Equals(std::vector<int>{1010}) );
   }

   SECTION("one once timer canceled before time is up") {
      std::vector<int> callsAtMillis;
      time.mMilliseconds = 1000;

      auto timer = executionContext.callOnce(milliseconds(10),[&callsAtMillis,&time]{
         callsAtMillis.push_back(time.milliseconds());
      });

      countUpTo(1009);

      timer->cancel();

      countUpTo(2000);
    
      REQUIRE(callsAtMillis.empty());
   }

   SECTION("two periodic timers") {
      std::vector<int> oneCallsAtMillis;
      std::vector<int> twoCallsAtMillis;
      

      time.mMilliseconds = 1000;

      executionContext.callPeriodically(milliseconds(100), [&oneCallsAtMillis, &time](auto& pTimer){
         oneCallsAtMillis.push_back(time.milliseconds());
      });

      executionContext.callPeriodically(milliseconds(250), [&twoCallsAtMillis, &time](auto& pTimer){
         twoCallsAtMillis.push_back(time.milliseconds());
      });

      countUpTo(1550);

      REQUIRE_THAT(oneCallsAtMillis, Catch::Equals(std::vector<int>{1100, 1200, 1300, 1400, 1500}) );
      REQUIRE_THAT(twoCallsAtMillis, Catch::Equals(std::vector<int>{1250, 1500}) );

   } 

   SECTION("two periodic timers with steps that 'overstep'") {
      std::vector<int> oneCallsAtMillis;
      std::vector<int> twoCallsAtMillis;
      

      time.mMilliseconds = 1000;

      executionContext.callPeriodically(milliseconds(100), [&oneCallsAtMillis, &time](auto& pTimer){
         oneCallsAtMillis.push_back(time.milliseconds());
      });

      executionContext.callPeriodically(milliseconds(250), [&twoCallsAtMillis, &time](auto& pTimer){
         twoCallsAtMillis.push_back(time.milliseconds());
      });

      countUpTo(1550, 27);

      REQUIRE_THAT(oneCallsAtMillis, Catch::Equals(std::vector<int>{1108, 1216, 1324, 1405, 1513}) );
      REQUIRE_THAT(twoCallsAtMillis, Catch::Equals(std::vector<int>{1270, 1513}) );

   } 

   SECTION("mix two periodic and once timer") {
      std::vector<int> oneCallsAtMillis;
      std::vector<int> twoCallsAtMillis;
      std::vector<int> threeCallsAtMillis;
      

      time.mMilliseconds = 1000;

      executionContext.callPeriodically(milliseconds(100), [&oneCallsAtMillis, &time](auto& pTimer){
         oneCallsAtMillis.push_back(time.milliseconds());
      });

      executionContext.callPeriodically(milliseconds(250), [&twoCallsAtMillis, &time](auto& pTimer){
         twoCallsAtMillis.push_back(time.milliseconds());
      });

      countUpTo(1150);

      executionContext.callOnce(seconds(60), [&threeCallsAtMillis, &time](){
         threeCallsAtMillis.push_back(time.milliseconds());
      });

      countUpTo(1550);

      REQUIRE_THAT(oneCallsAtMillis, Catch::Equals(std::vector<int>{1100, 1200, 1300, 1400, 1500}) );
      REQUIRE_THAT(twoCallsAtMillis, Catch::Equals(std::vector<int>{1250, 1500}) );

      countUpTo(80000, 100);
      REQUIRE_THAT(threeCallsAtMillis, Catch::Equals(std::vector<int>{61150}) );
   } 

   SECTION("trigger once timer in callback of periodic timer") {
      std::vector<int> oneCallsAtMillis;
      std::vector<int> twoCallsAtMillis;
      std::vector<int> threeCallsAtMillis;
      

      time.mMilliseconds = 1000;

      executionContext.callPeriodically(milliseconds(100), [&oneCallsAtMillis, &time](auto& pTimer){
         oneCallsAtMillis.push_back(time.milliseconds());
      });

      executionContext.callPeriodically(milliseconds(250), [&twoCallsAtMillis, &executionContext ,&threeCallsAtMillis, &time](auto& pTimer){
         if (twoCallsAtMillis.empty()) {
            executionContext.callOnce(seconds(60), [&threeCallsAtMillis, &time](){
               threeCallsAtMillis.push_back(time.milliseconds());
            });
         }
         twoCallsAtMillis.push_back(time.milliseconds());
         
      });

      countUpTo(1550);

      REQUIRE_THAT(oneCallsAtMillis, Catch::Equals(std::vector<int>{1100, 1200, 1300, 1400, 1500}) );
      REQUIRE_THAT(twoCallsAtMillis, Catch::Equals(std::vector<int>{1250, 1500}) );

      countUpTo(80000, 100);
      REQUIRE_THAT(threeCallsAtMillis, Catch::Equals(std::vector<int>{61250}));
   } 



   SECTION("call => future only available after fisrt workcycle") {
      auto future = executionContext.callTask([](){
         return 4711;
      });

      REQUIRE(std::future_status::timeout == future.wait_for(std::chrono::milliseconds(10))); 

      executionContext.doWorkcycle();
      
      REQUIRE(std::future_status::ready == future.wait_for(std::chrono::milliseconds(10)));
      REQUIRE(4711 == future.get());
   }

   SECTION("multiple calls => futures and values available after one workcycle") {
      auto future1 = executionContext.callTask([](){
         return 1;
      });
      auto future2 = executionContext.callTask([](){
         return 2;
      });
      auto future3 = executionContext.callTask([](){
         return 3;
      });

      executionContext.doWorkcycle();
      
      REQUIRE(std::future_status::ready == future1.wait_for(std::chrono::milliseconds(10)));
      REQUIRE(std::future_status::ready == future2.wait_for(std::chrono::milliseconds(10)));
      REQUIRE(std::future_status::ready == future3.wait_for(std::chrono::milliseconds(10)));
      
      REQUIRE(1 == future1.get());
      REQUIRE(2 == future2.get());
      REQUIRE(3 == future3.get());
   }

   SECTION("multiple calls => futures and values available after one workcycle") {
      std::vector<int> callIds;
      
      executionContext.call([&callIds](){
         callIds.push_back(1);
      });
      executionContext.call([&callIds](){
         callIds.push_back(2);
      });
      executionContext.call([&callIds](){
         callIds.push_back(3);
      });

      executionContext.doWorkcycle();
      
      REQUIRE_THAT(callIds, Catch::Equals(std::vector<int>{1,2,3}) );

   }

#ifndef ESP32 // SOMEHOW the exception stuff is not reliably working in the EPS32 targets

   SECTION("call that throws domain_error") {
      auto future = executionContext.callTask([](){
         throw std::domain_error("just a domain_error exception");
         return true;
      });
      
      executionContext.doWorkcycle();
      REQUIRE_THROWS_AS(future.get(), std::domain_error);
   }

   SECTION("call that throws invalid_argument") {
      auto future = executionContext.callTask([](){
         throw std::invalid_argument("just a invalid_argument exception");
         return true;
      });
      
      executionContext.doWorkcycle();
      REQUIRE_THROWS_AS(future.get(), std::invalid_argument);
   }  

#endif // ifndef ESP32

}

} // namespace Concurrency
} // namespace Bt

