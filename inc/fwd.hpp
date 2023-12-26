#include <cstddef>
#include <utility>

#include "tuple.hpp"
#include "comparators.hpp"

namespace static_containers
{
    namespace detail
    {
        template < size_t FROM, size_t UNTIL, size_t I = 0, typename... Ordered >
        struct FwdSlicedImpl
        {
            template < typename F, typename FirstUnordered, typename... Unordered >
            constexpr static auto fwd(F && f,
             Ordered &&... ordered,
             FirstUnordered && to_order,
             Unordered &&... unordered)
            {
                if constexpr (I == UNTIL)
                {
                    return f(std::forward< Ordered >(ordered)...);
                }
                else if constexpr (I < FROM)
                {
                    return FwdSlicedImpl< FROM, UNTIL, I + 1, Ordered... >::fwd(
                     std::forward< F >(f),
                     std::forward< Ordered >(ordered)...,
                     std::forward< Unordered >(unordered)...);
                }
                else
                {
                    return FwdSlicedImpl< FROM, UNTIL, I + 1, Ordered..., FirstUnordered >::fwd(
                     std::forward< F >(f),
                     std::forward< Ordered >(ordered)...,
                     std::forward< FirstUnordered >(to_order),
                     std::forward< Unordered >(unordered)...);
                }
            }

            template < typename F >
            constexpr static auto fwd(F && f, Ordered &&... ordered)
            {
                static_assert(I == UNTIL);
                return f(std::forward< Ordered >(ordered)...);
            }
        };
    }

    template < size_t FROM, size_t UNTIL, typename F, typename... Args >
    constexpr auto fwd_sliced(F && f, Args &&... args)
    {
        static_assert(FROM <= UNTIL,
         "UNTIL idx should be greater then FROM idx or equal to FROM idx");
        return detail::FwdSlicedImpl< FROM, UNTIL >::fwd(std::forward< F >(f),
         std::forward< Args >(args)...);
    }

    template < size_t UNTIL, typename F, typename... Args >
    constexpr auto fwd_first(F && f, Args &&... args)
    {
        return fwd_sliced< 0, UNTIL >(std::forward< F >(f), std::forward< Args >(args)...);
    }

    template < size_t FROM, typename F, typename... Args >
    constexpr auto fwd_last(F && f, Args &&... args)
    {
        return fwd_sliced< FROM, sizeof...(Args) >(std::forward< F >(f),
         std::forward< Args >(args)...);
    }

    namespace detail
    {
        template < size_t DESIRED, size_t I = 0, typename F, typename FirstArg, typename... Args >
        constexpr auto fwd_ith_impl(F && f, FirstArg && arg, Args &&... args)
        {
            if constexpr (I == DESIRED)
            {
                return f(std::forward< FirstArg >(arg));
            }
            else
            {
                return fwd_ith_impl< DESIRED, I + 1 >(std::forward< F >(f),
                 std::forward< Args >(args)...);
            }
        }
    }

    template < size_t I, typename F, typename... Args >
    constexpr auto fwd_ith(F f, Args &&... args)
    {
        return detail::fwd_ith_impl< I, 0, F, Args... >(std::forward< F >(f),
         std::forward< Args >(args)...);
    }

    namespace detail
    {
        template < size_t I, size_t J, typename... OriginalTypelist >
        struct FwdSwappedImpl
        {
            static_assert(I < J);

            template < typename F, typename... Args >
            constexpr static auto fwd(F && f, Args &&... args)
            {
                auto begining = fwd_sliced< 0, I >(make_tuple, std::forward< Args >(args)...);
                auto i = fwd_ith< I >(make_tuple, std::forward< Args >(args)...);
                auto middle = fwd_sliced< I + 1, J >(make_tuple, std::forward< Args >(args)...);
                auto j = fwd_ith< J >(make_tuple, std::forward< Args >(args)...);
                auto ending =
                 fwd_sliced< J + 1, sizeof...(Args) >(make_tuple, std::forward< Args >(args)...);
                auto swapped_arglist = concat(begining, j, middle, i, ending);
                return unwrap_then_do(std::forward< F >(f), std::move(swapped_arglist));
            }
        };
    }

    template < size_t I, size_t J, typename F, typename... Args >
    constexpr auto fwd_swapped(F && f, Args &&... args)
    {
        return detail::FwdSwappedImpl< I, J, Args... >::fwd(std::forward< F >(f),
         std::forward< Args >(args)...);
    }

    namespace detail
    {
        template < size_t N, size_t I, typename TypeComparator, typename... Passed >
        struct FwdSortedImpl
        {
            template < typename F,
             typename FirstUnpassed,
             typename SecondUnpassed,
             typename... Unpassed >
            constexpr static auto fwd(F && f,
             Passed &&... passed,
             FirstUnpassed && to_pass1,
             SecondUnpassed && to_pass2,
             Unpassed &&... unpassed)
            {
                if constexpr (!TypeComparator::template value< decltype(to_pass1),
                               decltype(to_pass2) >())
                {
                    return FwdSortedImpl< N, I, TypeComparator, Passed..., SecondUnpassed >::fwd(
                     std::forward< F >(f),
                     std::forward< Passed >(passed)...,
                     std::forward< SecondUnpassed >(to_pass2),
                     std::forward< FirstUnpassed >(to_pass1),
                     std::forward< Unpassed >(unpassed)...);
                }
                else
                {
                    return FwdSortedImpl< N, I, TypeComparator, Passed..., FirstUnpassed >::fwd(
                     std::forward< F >(f),
                     std::forward< Passed >(passed)...,
                     std::forward< FirstUnpassed >(to_pass1),
                     std::forward< SecondUnpassed >(to_pass2),
                     std::forward< Unpassed >(unpassed)...);
                }
            }

            template < typename F, typename FirstUnpassed >
            constexpr static auto fwd(F && f, Passed &&... passed, FirstUnpassed && to_pass)
            {
                return FwdSortedImpl< N, I, TypeComparator, Passed..., FirstUnpassed >::fwd(
                 std::forward< F >(f),
                 std::forward< Passed >(passed)...,
                 std::forward< FirstUnpassed >(to_pass));
            }

            template < typename F >
            constexpr static auto fwd(F && f, Passed &&... passed)
            {
                if constexpr (I == N)
                {
                    return f(std::forward< Passed >(passed)...);
                }
                else
                {
                    return FwdSortedImpl< N, I + 1, TypeComparator >::fwd(std::forward< F >(f),
                     std::forward< Passed >(passed)...);
                }
            }
        };
    }

    template < typename TypeComparator, typename F, typename... Args >
    constexpr auto fwd_sorted(F && f, Args &&... args)
    {
        return detail::FwdSortedImpl< sizeof...(Args), 0, TypeComparator >::fwd(
         std::forward< F >(f),
         std::forward< Args >(args)...);
    }
}