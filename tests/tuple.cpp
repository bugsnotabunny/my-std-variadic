#include <catch2/catch_test_macros.hpp>
#include <cstddef>

#include "tuple.hpp"

namespace stct = static_containers;

TEST_CASE("tuple creation")
{
    auto tuple = stct::Tuple{ int{ 0 }, size_t{ 1 }, char{ 2 } };
    REQUIRE(tuple.at< 0 >() == 0);
    REQUIRE(tuple.at< 1 >() == 1);
    REQUIRE(tuple.at< 2 >() == 2);
};

TEST_CASE("tuple size")
{
    auto tuple = stct::Tuple{ int{ 0 }, size_t{ 1 }, char{ 2 } };
    REQUIRE(tuple.size() == 3);
};

TEST_CASE("tuple tie")
{
    auto tuple = stct::Tuple{ int{ 0 }, size_t{ 1 }, char{ 2 } };
    int a = 1;
    size_t b = 2;
    char c = 3;
    stct::tie(a, b, c) = tuple;
    REQUIRE(a == 0);
    REQUIRE(b == 1);
    REQUIRE(c == 2);
};
