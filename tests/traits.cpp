#include "traits.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstddef>

#include "comparators.hpp"

namespace stct = static_containers;

TEST_CASE("ith type")
{
    using t = stct::ith< 3, size_t, int, double, char, float, void >::type;
    REQUIRE(typeid(t).name() == typeid(char).name());
}

TEST_CASE("sorted pack")
{
    namespace cmp = stct::comparators;
    {
        using packed = stct::pack< size_t, int, double, char, float >;
        REQUIRE(!packed::is_sorted< cmp::size_of::less >());
    }
    {
        using packed = stct::pack< char, short, int, long, long long >;
        REQUIRE(packed::is_sorted< cmp::size_of::less >());
    }
}
