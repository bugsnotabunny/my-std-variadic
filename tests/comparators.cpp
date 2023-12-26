#include "comparators.hpp"

#include <catch2/catch_test_macros.hpp>

namespace stct = static_containers;
namespace cmp = stct::comparators;

TEST_CASE("sizeof cmp")
{
    REQUIRE(cmp::size_of::eq::value< char, char >());
    REQUIRE(!cmp::size_of::eq::value< char, int >());

    REQUIRE(cmp::size_of::neq::value< char, int >());
    REQUIRE(!cmp::size_of::neq::value< char, char >());

    REQUIRE(cmp::size_of::greater::value< int, char >());
    REQUIRE(!cmp::size_of::greater::value< char, int >());

    REQUIRE(cmp::size_of::less::value< char, int >());
    REQUIRE(!cmp::size_of::less::value< int, char >());

    REQUIRE(cmp::size_of::lesseq::value< char, int >());
    REQUIRE(cmp::size_of::lesseq::value< char, char >());
    REQUIRE(!cmp::size_of::lesseq::value< int, char >());

    REQUIRE(cmp::size_of::greatereq::value< int, char >());
    REQUIRE(cmp::size_of::greatereq::value< char, char >());
    REQUIRE(!cmp::size_of::greatereq::value< char, int >());
}

TEST_CASE("alignof cmp")
{
    REQUIRE(cmp::align_of ::eq::value< char, char >());
    REQUIRE(!cmp::align_of::eq::value< char, int >());

    REQUIRE(cmp::align_of::neq::value< char, int >());
    REQUIRE(!cmp::align_of::neq::value< char, char >());

    REQUIRE(cmp::align_of::greater::value< int, char >());
    REQUIRE(!cmp::align_of::greater::value< char, int >());

    REQUIRE(cmp::align_of::less::value< char, int >());
    REQUIRE(!cmp::align_of::less::value< int, char >());

    REQUIRE(cmp::align_of::lesseq::value< char, int >());
    REQUIRE(cmp::align_of::lesseq::value< char, char >());
    REQUIRE(!cmp::align_of::lesseq::value< int, char >());

    REQUIRE(cmp::align_of::greatereq::value< int, char >());
    REQUIRE(cmp::align_of::greatereq::value< char, char >());
    REQUIRE(!cmp::align_of::greatereq::value< char, int >());
}
