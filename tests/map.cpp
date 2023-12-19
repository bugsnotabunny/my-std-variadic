#include "map.hpp"

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

namespace stct = static_containers;

namespace
{
    struct Set0
    {
        constexpr void operator()() const noexcept
        {
            to_set = 0;
        }
        size_t & to_set;
    };

    void set_some(size_t & to_set, size_t some)
    {
        to_set = some;
    }
}

TEST_CASE("map access")
{
    size_t to_set = 1234567;
    using namespace std::placeholders;
    auto set2 = [&to_set]()
    {
        set_some(to_set, 2);
    };
    auto set3 = std::bind(set_some, std::ref(to_set), 3);
    auto map = stct::make_map< std::string >(stct::Tuple{ std::string("set0"), Set0(to_set) },
     stct::Tuple{ std::string("set2"), set2 },
     stct::Tuple{ std::string("set3"), set3 });

    using namespace stct::visitors;

    call_at(map, "set0");
    REQUIRE(to_set == 0);
    call_at(map, "set2");
    REQUIRE(to_set == 2);
    call_at(map, "set3");
    REQUIRE(to_set == 3);
}

TEST_CASE("map incr decr")
{
    auto map = stct::make_map< int >(stct::Tuple{ 0, int(1) },
     stct::Tuple{ 1, short(1) },
     stct::Tuple{ 2, char(1) });

    using namespace stct::visitors;
    for (int i = 0; i < 3; ++i)
    {
        decr_at(map, i);
        REQUIRE(map.visit_at(i,
         [](auto & elem)
         {
             return elem == 0;
         }));
    }

    for (int i = 0; i < 3; ++i)
    {
        incr_at(map, i);
        REQUIRE(map.visit_at(i,
         [](auto & elem)
         {
             return elem == 1;
         }));
    }
}
