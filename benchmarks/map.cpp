#include "map.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <map>

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

    constexpr void set_some(size_t & to_set, size_t some) noexcept
    {
        to_set = some;
    }
}

TEST_CASE("map benchmarking")
{
    size_t to_set = 1234567;
    using namespace std::placeholders;
    auto set2 = [&to_set]()
    {
        set_some(to_set, 2);
    };
    auto set3 = std::bind(set_some, std::ref(to_set), 3);

    BENCHMARK("std map")
    {

        static const auto map =
         std::map< std::string, std::function< void() > >{ { "set0", Set0(to_set) },
             { "set2", set2 },
             { "set3", set3 } };
        map.at("set0")();
        map.at("set2")();
        return map.at("set3")();
    };

    BENCHMARK("stct map")
    {
        auto set3 = std::bind(set_some, std::ref(to_set), 3);
        static const auto map =
         stct::make_map< std::string >(stct::Tuple{ std::string("set0"), Set0(to_set) },
          stct::Tuple{ std::string("set2"), set2 },
          stct::Tuple{ std::string("set3"), set3 });
        using namespace stct::visitors;
        call_at(map, "set0");
        call_at(map, "set2");
        return call_at(map, "set3");
    };
}