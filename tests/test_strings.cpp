#include <catch2/catch_test_macros.hpp>
#include "src/String.hpp"

TEST_CASE( "wwc::Strings", "[contains]" ) {
    wwc::String str{"good night"};
    
    REQUIRE(str.contains("good")==true);
    REQUIRE_FALSE(str.contains("good  "));
}
