#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <type_traits>

#include "variant.hpp"

TEST_CASE("empty default variant")
{
    static_containers::Variant< int, bool > var;
    REQUIRE(typeid(var.at< 0 >()) == typeid(static_containers::VariantEmpty));
}

TEST_CASE("set-get variant idx")
{
    static_containers::Variant< int, bool > var;
    var = int{ 2 };
    REQUIRE(typeid(var.at< 1 >()) == typeid(int));
    REQUIRE(var.at< 1 >() == 2);
    var = bool{ true };
    REQUIRE(typeid(var.at< 2 >()) == typeid(bool));
    REQUIRE(var.at< 2 >() == true);
}

TEST_CASE("get variant idx bad")
{
    static_containers::Variant< int, bool > var;
    REQUIRE_THROWS(var.template at< 2 >());
}

TEST_CASE("set-get variant type")
{
    static_containers::Variant< int, bool > var;
    var = int{ 2 };
    REQUIRE(typeid(var.template at< 1 >()) == typeid(int));
    REQUIRE(var.at_t< int >() == 2);

    var = bool{ true };
    REQUIRE(typeid(var.template at< 2 >()) == typeid(bool));
    REQUIRE(var.at_t< bool >() == true);
}

TEST_CASE("get variant type bad")
{
    static_containers::Variant< int, bool > var;
    REQUIRE_THROWS(var.template at_t< bool >());
}

TEST_CASE("visit variant")
{
    static_containers::Variant< int, bool > var;
    var = int{ 100 };
    REQUIRE_THROWS(static_containers::visit(var,
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
    using VarT = static_containers::Variant< int, bool >;
    VarT var = int{ 100 };
    auto new_var = static_containers::visit(var,
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