#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "test-premake-project/test-premake-project.h"

TEST_CASE("Example class tests", "[Example]") {
    SECTION("Constructor") {
        test-premake-project::Example example;
        REQUIRE(example.getValue() == 0);

        test-premake-project::Example example2(42);
        REQUIRE(example2.getValue() == 42);
    }

    SECTION("Get and set value") {
        test-premake-project::Example example;
        example.setValue(100);
        REQUIRE(example.getValue() == 100);
    }
}

TEST_CASE("Free function tests", "[Functions]") {
    SECTION("add function") {
        REQUIRE(test-premake-project::add(3, 4) == 7);
        REQUIRE(test-premake-project::add(-1, 1) == 0);
        REQUIRE(test-premake-project::add(0, 0) == 0);
    }
}

TEST_CASE("Version tests", "[Version]") {
    SECTION("version is not empty") {
        REQUIRE_FALSE(test-premake-project::getVersion().empty());
    }
}
