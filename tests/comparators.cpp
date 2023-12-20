#include "comparators.hpp"

#include <catch2/catch_test_macros.hpp>

namespace stct = static_containers;
namespace cmp = stct::comparators;

TEST_CASE("sizeof cmp")
{
    REQUIRE(cmp::size_of::eq_v< char, char >);
    REQUIRE(!cmp::size_of::eq_v< char, int >);

    REQUIRE(cmp::size_of::neq_v< char, int >);
    REQUIRE(!cmp::size_of::neq_v< char, char >);

    REQUIRE(cmp::size_of::grearer_v< int, char >);
    REQUIRE(!cmp::size_of::grearer_v< char, int >);

    REQUIRE(cmp::size_of::less_v< char, int >);
    REQUIRE(!cmp::size_of::less_v< int, char >);

    REQUIRE(cmp::size_of::lesseq_v< char, int >);
    REQUIRE(cmp::size_of::lesseq_v< char, char >);
    REQUIRE(!cmp::size_of::lesseq_v< int, char >);

    REQUIRE(cmp::size_of::greatereq_v< int, char >);
    REQUIRE(cmp::size_of::greatereq_v< char, char >);
    REQUIRE(!cmp::size_of::greatereq_v< char, int >);
}

TEST_CASE("alignof cmp")
{
    REQUIRE(cmp::align_of ::eq_v< char, char >);
    REQUIRE(!cmp::align_of::eq_v< char, int >);

    REQUIRE(cmp::align_of::neq_v< char, int >);
    REQUIRE(!cmp::align_of::neq_v< char, char >);

    REQUIRE(cmp::align_of::grearer_v< int, char >);
    REQUIRE(!cmp::align_of::grearer_v< char, int >);

    REQUIRE(cmp::align_of::less_v< char, int >);
    REQUIRE(!cmp::align_of::less_v< int, char >);

    REQUIRE(cmp::align_of::lesseq_v< char, int >);
    REQUIRE(cmp::align_of::lesseq_v< char, char >);
    REQUIRE(!cmp::align_of::lesseq_v< int, char >);

    REQUIRE(cmp::align_of::greatereq_v< int, char >);
    REQUIRE(cmp::align_of::greatereq_v< char, char >);
    REQUIRE(!cmp::align_of::greatereq_v< char, int >);
}
