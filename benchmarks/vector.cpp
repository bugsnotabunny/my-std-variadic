#include "vector.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

namespace stct = static_containers;

namespace
{
    auto vectorBenchmark(auto & vec)
    {
        vec.assign(500, 10);
        for (size_t i = 0; i < 250; ++i)
        {
            vec.push_back(10);
        }
        for (size_t i = 0; i < 250; ++i)
        {
            vec.pop_back();
        }
        return vec.erase(vec.begin() + 123, vec.begin() + 322);
    }
}

TEST_CASE("vector benchmarking")
{
    BENCHMARK("std vector")
    {
        std::vector< double > vec;
        vec.reserve(1000);
        return vectorBenchmark(vec);
    };

    BENCHMARK("stct vector")
    {
        stct::Vector< double, 1000 > vec;
        return vectorBenchmark(vec);
    };
}