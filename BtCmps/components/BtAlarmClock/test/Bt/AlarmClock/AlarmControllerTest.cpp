//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <Bt/Core/Timezone.h>
#include <Bt/Core/TimeMock.h>
#include <Bt/Concurrency/SchedulingExecutionContextStub.h>

#include "Bt/AlarmClock/AlarmController.h"
#include "Bt/AlarmClock/AlarmActorMock.h"

using namespace std::chrono_literals;
using namespace date;
using namespace date;

namespace Bt {
namespace AlarmClock {

TEST_CASE( "AlarmControllerTest", "[AlarmControllerTest]" ) {
   Core::TimeMock timeMock;
   Core::Timezone timezone;
   Concurrency::SchedulingExecutionContextStub executionContext;
   AlarmActorMock alarmActor;
   AlarmController mAlarmController(executionContext, timeMock, timezone, alarmActor);

   timeMock.mNow = sys_days{2019_y/January/1} + 10h + 44min;

   SECTION("check with no alarm") {
      REQUIRE_FALSE(mAlarmController.nextInLocal());    
   }

   SECTION("check with empty json") {
      const char* input = R"JSON(
         {
         }
      )JSON";

      REQUIRE(mAlarmController.load(input));
      REQUIRE_FALSE(mAlarmController.nextInLocal());    
   }

   SECTION("check with empty alarms json") {
      const char* input = R"JSON(
         {
            "repeatingAlarms" : []
         }
      )JSON";

      REQUIRE(mAlarmController.load(input));
      REQUIRE_FALSE(mAlarmController.nextInLocal());    
   }

   SECTION("check with two repeating alarms with different times on different days") {
      
      const char* input = R"JSON(
         {
            "repeatingAlarms" : [
               {"e":true, "h":5, "m":30, "days":["wed", "thu", "fri"]},
               {"e":true, "h":6, "m":30, "days":["mon", "tue"]}
            ]
         }
      )JSON";

      timeMock.mNow =  sys_days{2019_y/May/11} + 10h + 44min;
      REQUIRE(mAlarmController.load(input));
      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2019_y/May/13} + 6h + 30min));
      Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();

      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      // Mon 13 May
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2019_y/May/13} + 4h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2019_y/May/13} + 4h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      // Tue 14 May 
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2019_y/May/14} + 4h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2019_y/May/14} + 4h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      // Wed 15 May 
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2019_y/May/15} + 3h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }   
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2019_y/May/15} + 3h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      // Thu 16 May 
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2019_y/May/16} + 3h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }   
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2019_y/May/16} + 3h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
   }   

   SECTION("check with two once alarms") {
      
      const char* input = R"JSON(
         {
            "onceAlarms" : [
                {"e":true, "h": 8, "m":0, "D":25, "M":1, "Y":2020},
                {"e":true, "h": 13, "m":30, "D":26, "M":1, "Y":2020}
            ]
         }
      )JSON";

      timeMock.mNow =  sys_days{2020_y/January/24} + 10h + 44min;
      REQUIRE(mAlarmController.load(input));
      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/25} + 8h));
      Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();

      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      } 
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/25} + 7h;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/25} + 7h + 1min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/26} + 13h + 30min));
      
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/26} + 12h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      } 
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/26} + 12h + 31min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/26} + 12h + 32min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE_FALSE(mAlarmController.nextInLocal());

   }  

   SECTION("check with two once alarms that are already over") {
      
      const char* input = R"JSON(
         {
            "onceAlarms" : [
                {"e":true, "h": 8, "m":0, "D":25, "M":1, "Y":2019},
                {"e":true, "h": 13, "m":30, "D":26, "M":1, "Y":2019}
            ]
         }
      )JSON";

      timeMock.mNow =  sys_days{2020_y/January/24} + 10h + 44min;
      REQUIRE(mAlarmController.load(input));
      REQUIRE_FALSE(mAlarmController.nextInLocal());
   }

   SECTION("check with mixed alarms with different times on different days") {
      
      const char* input = R"JSON(
         {
            "repeatingAlarms" : [
               {"e":true, "h":5, "m":30, "days":["thu", "fri"]},
               {"e":true, "h":6, "m":30, "days":["mon", "tue"]}
            ],
            "onceAlarms" : [
                {"e":true, "h":  8, "m": 0, "D":22, "M":1, "Y":2020},
                {"e":true, "h": 13, "m":15, "D":20, "M":1, "Y":2020}
            ]
         }
      )JSON";

      timeMock.mNow =  sys_days{2020_y/January/18} + 10h + 44min;
      REQUIRE(mAlarmController.load(input));
      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/20} + 6h + 30min));
      Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();

      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      // Mon 20 January
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/20} + 5h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/20} + 5h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/20} + 13h + 15min));
      
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/20} + 12h + 14min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/20} + 12h + 15min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/21} + 6h + 30min));

      // Tue 21 January
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/21} + 5h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/21} + 5h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/22} + 8h));
      
      // Wed 22 January
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/22} + 6h + 59min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/22} + 7h;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/23} + 5h + 30min));
      
      // Thu 23 January
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/23} + 4h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/23} + 4h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/24} + 5h + 30min));

      // Fri 24 January
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/24} + 4h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/24} + 4h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/27} + 6h + 30min));

      // Mon 27 January
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/27} + 5h + 29min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }  
      {
         REQUIRE_CALL(alarmActor, activateAlarm(trompeloeil::_));
         timeMock.mNow =  sys_days{2020_y/January/27} + 5h + 30min;
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }
      {
         FORBID_CALL(alarmActor, activateAlarm(trompeloeil::_));
         Events::publish(I_Clock::MinuteUpdate{}); // mAlarmController.checkLoop();
      }

      REQUIRE(mAlarmController.nextInLocal().value() == (sys_days{2020_y/January/28} + 6h + 30min));

   }  
}

} // namespace AlarmClock
} // namespace Bt

