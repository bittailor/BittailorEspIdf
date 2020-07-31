//*************************************************************************************************
//
//  BITTAILOR.CH
//
//*************************************************************************************************

#include <catch.hpp>

#include "Bt/Protocols/Mqtt/Mqtt.h"

namespace Bt {
namespace Protocols {
namespace Mqtt {

TEST_CASE( "split MQTT topics", "[MqttTest]" ) {

   SECTION("split topic with 3 single entries") {
      auto splited = split("A/B/C");
      std::vector<std::string> exp{"A","B","C"};
      REQUIRE_THAT(splited, Catch::Matchers::Equals(exp));
   }

   SECTION("split topic with 3 entries") {
      auto splited = split("One/Two/Three");
      std::vector<std::string> exp{"One","Two","Three"};
      REQUIRE_THAT(splited, Catch::Matchers::Equals(exp));
   }

   SECTION("split topic with 3 entries") {
         auto splited = split("One/Two");
         std::vector<std::string> exp{"One","Two"};
         REQUIRE_THAT(splited, Catch::Matchers::Equals(exp));
      }

   SECTION("split topic with 1 entries") {
      auto splited = split("One");
      std::vector<std::string> exp{"One"};
      REQUIRE_THAT(splited, Catch::Matchers::Equals(exp));
   }

   SECTION("split empty topic") {
      auto splited = split("");
      REQUIRE(splited.size() == 0);
   }

   SECTION("split topic with 3 entries (+ wildcards)") {
      auto splited = split("One/+/Three");
      std::vector<std::string> exp{"One","+","Three"};
      REQUIRE_THAT(splited, Catch::Matchers::Equals(exp));
   }

   SECTION("split topic with 3 entries (* wildcards)") {
      auto splited = split("One/Two/*");
      std::vector<std::string> exp{"One","Two","*"};
      REQUIRE_THAT(splited, Catch::Matchers::Equals(exp));
   }

}

TEST_CASE( "MQTT topics matching subscription", "[MqttTest]" ) {

   SECTION("no wildcards") {
      REQUIRE(topicMatchesSubscription("A","A"));
      REQUIRE(topicMatchesSubscription("A/B","A/B"));
      REQUIRE(topicMatchesSubscription("A/B/C","A/B/C"));
      REQUIRE(topicMatchesSubscription("A/B/C/D","A/B/C/D"));
   }

   SECTION("+ wildcards") {
      REQUIRE(topicMatchesSubscription("A/+/C","A/B/C"));
      REQUIRE(topicMatchesSubscription("A/+/C","A/A/C"));
      REQUIRE(topicMatchesSubscription("A/+/C","A/Blue/C"));

      REQUIRE(topicMatchesSubscription("+/B/C","A/B/C"));
      REQUIRE(topicMatchesSubscription("+/B/C","B/B/C"));
      REQUIRE(topicMatchesSubscription("+/B/C","Blue/B/C"));

      REQUIRE(topicMatchesSubscription("A/B/+","A/B/C"));
      REQUIRE(topicMatchesSubscription("A/B/+","A/B/A"));
      REQUIRE(topicMatchesSubscription("A/B/+","A/B/Blue"));

      REQUIRE(topicMatchesSubscription("+/B/+/D","A/B/C/D"));
      REQUIRE(topicMatchesSubscription("+/B/+/D","B/B/A/D"));
      REQUIRE(topicMatchesSubscription("+/B/+/D","Blue/B/Black/D"));

   }

   SECTION("# wildcards") {
      REQUIRE(topicMatchesSubscription("A/#","A/B"));
      REQUIRE(topicMatchesSubscription("A/#","A/B/C"));
      REQUIRE(topicMatchesSubscription("A/#","A/B/C/D"));

      REQUIRE(topicMatchesSubscription("#","A"));
      REQUIRE(topicMatchesSubscription("#","A/B"));
      REQUIRE(topicMatchesSubscription("#","A/B/C"));
      REQUIRE(topicMatchesSubscription("#","A/B/C/D"));


      REQUIRE(topicMatchesSubscription("A/B/#","A/B"));
      REQUIRE(topicMatchesSubscription("A/B/#","A/B"));
      REQUIRE(topicMatchesSubscription("A/B/#","A/B/C"));
      REQUIRE(topicMatchesSubscription("A/B/#","A/B/C/D"));

   }


}

} // namespace Mqtt
} // namespace Protocols
} // namespace Bt
