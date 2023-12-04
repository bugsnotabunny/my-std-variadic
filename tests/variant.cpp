#include <catch2/catch_test_macros.hpp>

#include <type_traits>

#include "variant.hpp"

TEST_CASE("empty default variant")
{
    mystd::Variant< int, bool > var;
    REQUIRE(typeid(var.at< 0 >()) == typeid(mystd::VariantEmpty));
}

TEST_CASE("set-get variant idx")
{
    mystd::Variant< int, bool > var;
    var = int{ 2 };
    REQUIRE(typeid(var.at< 1 >()) == typeid(int));
    REQUIRE(var.at< 1 >() == 2);
    var = bool{ true };
    REQUIRE(typeid(var.at< 2 >()) == typeid(bool));
    REQUIRE(var.at< 2 >() == true);
}

TEST_CASE("get variant idx bad")
{
    mystd::Variant< int, bool > var;
    REQUIRE_THROWS(var.template at< 2 >());
}

TEST_CASE("set-get variant type")
{
    mystd::Variant< int, bool > var;
    var = int{ 2 };
    REQUIRE(typeid(var.template at< 1 >()) == typeid(int));
    REQUIRE(var.at_t< int >() == 2);

    var = bool{ true };
    REQUIRE(typeid(var.template at< 2 >()) == typeid(bool));
    REQUIRE(var.at_t< bool >() == true);
}

TEST_CASE("get variant type bad")
{
    mystd::Variant< int, bool > var;
    REQUIRE_THROWS(var.template at_t< bool >());
}
