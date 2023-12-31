#include "variant.hpp"

#include <cassert>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "drop_logger.hpp"

namespace stct = static_containers;

TEST_CASE("default variant")
{
    stct::Variant< int, bool > var;
    REQUIRE(var.at< 0 >() == 0);
}

TEST_CASE("set-get variant idx")
{
    stct::Variant< int, bool > var;
    var = int{ 2 };
    REQUIRE(typeid(var.at< 0 >()) == typeid(int));
    REQUIRE(var.at< 0 >() == 2);
    var = bool{ true };
    REQUIRE(typeid(var.at< 1 >()) == typeid(bool));
    REQUIRE(var.at< 1 >() == true);
}

TEST_CASE("get variant idx bad")
{
    stct::Variant< int, bool > var;
    REQUIRE_THROWS(var.at< 1 >());
}

TEST_CASE("set-get variant type")
{
    stct::Variant< int, bool > var;
    var = int{ 2 };
    REQUIRE(typeid(var.template at< 0 >()) == typeid(int));
    REQUIRE(var.at_t< int >() == 2);

    var = bool{ true };
    REQUIRE(typeid(var.at< 1 >()) == typeid(bool));
    REQUIRE(var.at_t< bool >() == true);
}

TEST_CASE("get variant type bad")
{
    stct::Variant< int, bool > var;
    REQUIRE_THROWS(var.template at_t< bool >());
}

TEST_CASE("visit variant")
{
    stct::Variant< int, bool > var;
    var = int{ 100 };
    REQUIRE_THROWS(stct::visit(var,
     [](auto & val)
     {
         if constexpr (std::is_same_v< int &, decltype(val) >)
         {
             throw std::logic_error("OK");
         }
     }));
}

TEST_CASE("visit variant return")
{
    using VarT = stct::Variant< int, bool >;
    VarT var = int{ 100 };
    auto new_var = stct::visit(var,
     [](auto & val) -> VarT
     {
         if constexpr (std::is_same_v< int &, decltype(val) >)
         {
             return { val };
         }
         else
         {
             return { true };
         }
     });
    REQUIRE(new_var.holds< int >());
}

TEST_CASE("variant destructs")
{
    using stct::testing::DropLogger;
    size_t destructed = 0;
    {
        stct::Variant< DropLogger, bool > to_destroy{ DropLogger{ destructed } };
    }
    assert(destructed == 1);
}