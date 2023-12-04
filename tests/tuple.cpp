#include <catch2/catch_test_macros.hpp>
#include <cstddef>

#include "tuple.hpp"

TEST_CASE("tuple creation")
{
    auto tuple = mystd::Tuple{ int{ 0 }, size_t{ 1 }, char{ 2 } };
    REQUIRE(tuple.at< 0 >() == 0);
    REQUIRE(tuple.at< 1 >() == 1);
    REQUIRE(tuple.at< 2 >() == 2);
};

TEST_CASE("tuple size")
{
    auto tuple = mystd::Tuple{ int{ 0 }, size_t{ 1 }, char{ 2 } };
    REQUIRE(tuple.size() == 3);
};

TEST_CASE("tuple tie")
{
    auto tuple = mystd::Tuple{ int{ 0 }, size_t{ 1 }, char{ 2 } };
    int a = 1;
    size_t b = 2;
    char c = 3;
    mystd::tie(a, b, c) = tuple;
    REQUIRE(a == 0);
    REQUIRE(b == 1);
    REQUIRE(c == 2);
};

TEST_CASE("tuple foreach")
{
    auto tuple = mystd::Tuple{ int{ 0 }, size_t{ 1 }, char{ 2 } };
    mystd::for_each(tuple.view_full(),
     [](auto & elem)
     {
         elem *= 2;
     });
    REQUIRE(tuple.at< 0 >() == 0);
    REQUIRE(tuple.at< 1 >() == 2);
    REQUIRE(tuple.at< 2 >() == 4);
};

TEST_CASE("tuple view shrink")
{
    auto tuple = mystd::Tuple{ int{ 1 }, size_t{ 1 }, char{ 1 }, short{ 1 } };
    mystd::for_each(tuple.view< 1, 3 >(),
     [](auto & elem)
     {
         elem *= 2;
     });
    REQUIRE(tuple.at< 0 >() == 1);
    REQUIRE(tuple.at< 1 >() == 2);
    REQUIRE(tuple.at< 2 >() == 2);
    REQUIRE(tuple.at< 3 >() == 1);
};
