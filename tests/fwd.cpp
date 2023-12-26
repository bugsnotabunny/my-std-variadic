#include "fwd.hpp"
#include "tuple.hpp"
#include "tuple_utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <cstddef>
#include <functional>
#include <utility>

namespace stct = static_containers;

namespace
{
    const auto ARGS = stct::Tuple{ int(10), unsigned(20), float(1.22), long(-1234), size_t(100) };
}

namespace
{
    template < size_t I, typename... Args >
    auto ith_square(Args... args)
    {
        return stct::fwd_ith< I >(
         [](auto num)
         {
             return num * num;
         },
         std::move(args)...);
    };
}

TEST_CASE("fwd ith")
{
    auto args_tup = ARGS;
    auto res = stct::unwrap_then_do(
     [](auto &&... args)
     {
         return ith_square< 3 >(std::move(args)...);
     },
     std::move(args_tup));

    REQUIRE(res == ARGS.at< 3 >() * ARGS.at< 3 >());
}

namespace
{
    template < size_t FROM, size_t UNTIL, typename... Args >
    auto slice_double(Args... args)
    {
        return stct::fwd_sliced< FROM, UNTIL >(
         [](auto &&... nums)
         {
             return stct::make_tuple((nums * 2)...);
         },
         std::move(args)...);
    };
}

TEST_CASE("fwd slice")
{
    auto args_tup = ARGS;
    auto res = stct::unwrap_then_do(
     [](auto &&... args)
     {
         return slice_double< 1, 4 >(std::move(args)...);
     },
     std::move(args_tup));

    REQUIRE(res == stct::Tuple{ ARGS.at< 1 >() * 2, ARGS.at< 2 >() * 2, ARGS.at< 3 >() * 2 });
}

TEST_CASE("fwd swap")
{
    auto args_tup = ARGS;
    auto res = stct::unwrap_then_do(
     [](auto &&... args)
     {
         return stct::fwd_swapped< 1, 3 >(stct::make_tuple, std::move(args)...);
     },
     std::move(args_tup));
    REQUIRE(res == stct::Tuple{
                    ARGS.at< 0 >(),
                    ARGS.at< 3 >(),
                    ARGS.at< 2 >(),
                    ARGS.at< 1 >(),
                    ARGS.at< 4 >(),
                   });
}