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
        void operator()() noexcept
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
    auto map = stct::make_map< std::string >(stct::Tuple{ std::string("set0"), Set0{ to_set } },
     stct::Tuple{ std::string("set2"), set2 },
     stct::Tuple{ std::string("set3"), set3 });

    map.call_at("set0");
    REQUIRE(to_set == 0);
    map.call_at("set2");
    REQUIRE(to_set == 2);
    map.call_at("set3");
    REQUIRE(to_set == 3);
}
