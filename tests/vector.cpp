#include "vector.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <ranges>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "drop_logger.hpp"

namespace stct = static_containers;

namespace
{
    const stct::Vector< int, 3 > TEST_DEFAULT{ 0, 1, 2 };
}

TEST_CASE("empty vector")
{
    stct::Vector< int, 2000 > vec;
    REQUIRE(vec.empty());
}

TEST_CASE("initialize vector from count")
{
    stct::Vector< int, 3 > vec;
    vec.assign(3, 23);
    for (auto && elem: vec)
    {
        REQUIRE(elem == 23);
    }
}

TEST_CASE("initialize vector from iters")
{
    std::vector< int > src = { 0, 1, 2 };
    stct::Vector< int, 3 > dest;
    dest.assign(src.begin(), src.end());
    REQUIRE(dest[0] == 0);
    REQUIRE(dest[1] == 1);
    REQUIRE(dest[2] == 2);
}

TEST_CASE("initialize vector from init list")
{
    stct::Vector< int, 3 > vec;
    vec.assign({ 0, 1, 2 });
    REQUIRE(vec[0] == 0);
    REQUIRE(vec[1] == 1);
    REQUIRE(vec[2] == 2);
}

TEST_CASE("vector destruction")
{
    using stct::testing::DropLogger;
    static size_t destructed = 0;
    auto logger = DropLogger{ destructed };
    {
        stct::Vector< DropLogger, 10 > vec{ 10, logger };
    }
    REQUIRE(destructed == 10);
    destructed = 0;
}

TEST_CASE("vector at")
{
    stct::Vector a(TEST_DEFAULT);
    REQUIRE(a.at(2) == 2);
    REQUIRE_THROWS(a.at(134134));
}

TEST_CASE("vector front")
{
    stct::Vector a(TEST_DEFAULT);
    REQUIRE(a.back() == 2);
    REQUIRE(a.front() == 0);

    const stct::Vector b(TEST_DEFAULT);
    REQUIRE(b.back() == 2);
    REQUIRE(b.front() == 0);
}

TEST_CASE("vector iteration")
{
    stct::Vector a(TEST_DEFAULT);
    int i = 0;
    for (auto && elem: a)
    {
        REQUIRE(elem == i);
        ++i;
    }

    const stct::Vector b(TEST_DEFAULT);
    int j = 0;
    for (auto && elem: b)
    {
        REQUIRE(elem == j);
        ++j;
    }
}

TEST_CASE("vector rev iteration")
{
    stct::Vector a(TEST_DEFAULT);
    int i = 2;
    for (auto && elem: std::views::reverse(a))
    {
        REQUIRE(elem == i);
        --i;
    }

    const stct::Vector b(TEST_DEFAULT);
    int j = 2;
    for (auto && elem: std::views::reverse(b))
    {
        REQUIRE(elem == j);
        --j;
    }
}

TEST_CASE("vector pop_back")
{
    using stct::testing::DropLogger;
    static size_t destructed = 0;
    auto logger = DropLogger{ destructed };
    stct::Vector< DropLogger, 3 > a(3, logger);
    REQUIRE(a.size() == 3);
    a.pop_back();
    REQUIRE(a.size() == 2);
    REQUIRE(destructed == 1);
}

TEST_CASE("vector clear")
{
    using stct::testing::DropLogger;
    static size_t destructed = 0;
    auto logger = DropLogger{ destructed };
    stct::Vector< DropLogger, 3 > a(3, logger);
    a.clear();
    REQUIRE(a.empty());
    REQUIRE(destructed == 3);
}

TEST_CASE("vector resize")
{
    SECTION("shrink")
    {
        using stct::testing::DropLogger;
        static size_t destructed = 0;
        auto logger = DropLogger{ destructed };
        stct::Vector< DropLogger, 15 > a(13, logger);
        a.resize(4, logger);
        REQUIRE(destructed == 9);
        REQUIRE(a.size() == 4);
    }
    SECTION("grow")
    {
        stct::Vector< int, 15 > a(3, 1);
        a.resize(12, 2);
        REQUIRE(a.size() == 12);
        for (auto it = a.begin() + 3; it != a.end(); ++it)
        {
            REQUIRE(*it == 2);
        }
    }
}

TEST_CASE("vector insert")
{
    using VecT = stct::Vector< int, 100 >;
    VecT base(3, 1);
    SECTION("one value")
    {
        VecT expected = { 1, 2, 1, 1 };
        auto vec(base);
        auto it = vec.insert(vec.begin() + 1, 2);
        REQUIRE(*it == 2);
        REQUIRE(vec == expected);
    }
    SECTION("many values")
    {
        VecT expected = { 1, 2, 2, 2, 1, 1 };
        auto vec(base);
        auto it = vec.insert(vec.begin() + 1, 3, 2);
        REQUIRE(*it == 2);
        REQUIRE(vec == expected);
    }
    SECTION("init list")
    {
        VecT expected = { 1, 3, 4, 5, 1, 1 };
        auto vec(base);
        auto it = vec.insert(vec.begin() + 1, { 3, 4, 5 });
        REQUIRE(*it == 3);
        REQUIRE(vec == expected);
    }
}

TEST_CASE("vector erase")
{
    using VecT = stct::Vector< int, 100 >;
    SECTION("one value")
    {
        VecT expected = { 1, 1, 3 };
        VecT vec = { 1, 1, 2, 3 };
        auto it = vec.erase(vec.begin() + 2);
        REQUIRE(*it == 3);
        REQUIRE(vec == expected);
    }
    SECTION("iterator range")
    {
        VecT expected = { 1, 3 };
        VecT vec = { 1, 2, 2, 2, 3 };
        auto it = vec.erase(vec.begin() + 1, vec.end() - 1);
        REQUIRE(*it == 3);
        REQUIRE(vec == expected);
    }
    SECTION("erase non-trival type")
    {
        auto vec = stct::Vector< std::string, 20 >{ "dsaad", "aaaa", "auidisfou" };
        vec.erase(vec.begin());
    }
    SECTION("remove-erase")
    {
        VecT expected = { 4, 3 };
        VecT vec = { 4, 2, 2, 2, 3 };
        auto removed = stct::erase(vec, 2);
        REQUIRE(removed == 3);
        REQUIRE(vec == expected);
    }
    SECTION("remove-erase if")
    {
        VecT expected = { 3 };
        VecT vec = { 4, 2, 2, 2, 3 };
        auto removed = stct::erase_if(vec,
         [](const auto & elem)
         {
             return elem % 2 == 0;
         });
        REQUIRE(removed == 4);
        REQUIRE(vec == expected);
    }
}
