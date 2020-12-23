//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>

#include "Bt/Core/Singleton.h"

namespace Bt {
namespace Core {
namespace {

   class SingletonTestDummy{};

}

TEST_CASE( "SingletonTest", "[SingletonTest]" ) {

   SECTION("not instance created") {   
      REQUIRE_FALSE(Singleton<SingletonTestDummy>::valid());
   }  

   SECTION("instance created") { 
      SingletonTestDummy dummy;
      Singleton<SingletonTestDummy>::Instance dummySingletonInstance(dummy);  
      REQUIRE(Singleton<SingletonTestDummy>::valid());
      REQUIRE(&Singleton<SingletonTestDummy>::instance() == &dummy) ;
   }  

   SECTION("not instance created acces throws") {   
      REQUIRE_THROWS(Singleton<SingletonTestDummy>::instance());
   } 

   SECTION("scoped instances") { 
      {
         SingletonTestDummy dummyOne;
         Singleton<SingletonTestDummy>::Instance dummyOneSingletonInstance(dummyOne);  
         {
            SingletonTestDummy dummyTwo;
            Singleton<SingletonTestDummy>::Instance dummyTwoSingletonInstance(dummyTwo); 
            REQUIRE(Singleton<SingletonTestDummy>::valid());
            REQUIRE(&Singleton<SingletonTestDummy>::instance() == &dummyTwo) ;    
         }        
         REQUIRE(Singleton<SingletonTestDummy>::valid());
         REQUIRE(&Singleton<SingletonTestDummy>::instance() == &dummyOne) ;
      }
      REQUIRE_FALSE(Singleton<SingletonTestDummy>::valid());
   } 
   
}

} // namespace Core
} // namespace Bt

