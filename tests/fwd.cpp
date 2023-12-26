#include "fwd.hpp"
#include "comparators.hpp"
#include "tuple.hpp"
#include "tuple_utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <utility>

namespace stct = static_containers;

namespace
{
    const auto ARGS = stct::Tuple{ nullptr, long(-1234), int(10), char('b'), float(1.22) };
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

TEST_CASE("fwd sorted")
{
    auto args_tuple = ARGS;

    using sizeof_less = stct::comparators::size_of::less;
    auto res = stct::unwrap_then_do(
     [](auto &&... args)
     {
         return stct::fwd_sorted< sizeof_less >(stct::make_tuple, std::move(args)...);
     },
     std::move(args_tuple));

    REQUIRE(decltype(res)::args_as_pack::is_sorted< sizeof_less >());
}
