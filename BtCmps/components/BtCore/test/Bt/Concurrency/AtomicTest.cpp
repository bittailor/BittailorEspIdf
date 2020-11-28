//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>

#include <atomic>

namespace Bt {
namespace Concurrency {
namespace {
   class Dummy {
      public:
         Dummy(int pValue):mValue(pValue){}
         int value(){return mValue;}   
      private:
         int mValue;
   };

}

TEST_CASE( "AtomicTest", "[atomic]" ) {
   Dummy dummy1{1};
   Dummy dummy2{2};
   

   SECTION("check replace on null") {
      std::atomic<Dummy*> instance{nullptr};
      Dummy* expected = nullptr;
      REQUIRE(instance.compare_exchange_strong(expected,&dummy1));
      expected = nullptr;
      REQUIRE_FALSE(instance.compare_exchange_strong(expected,&dummy2));
      REQUIRE(expected == &dummy1);
      REQUIRE(instance == &dummy1);

   }   
   
}

} // namespace Concurrency
} // namespace Bt

