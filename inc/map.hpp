#ifndef STCT_MAP_HPP
#define STCT_MAP_HPP

#include <algorithm>
#include <array>
#include <bits/ranges_algo.h>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

#include "tuple.hpp"
#include "vector.hpp"

namespace static_containers
{
    namespace detail
    {
        template < typename T, typename U >
        concept Comparator = requires(T x, const U & rhs, const U & lhs) {
            {
                x(rhs, lhs)
            } -> std::convertible_to< bool >;
        };
    }

    template < typename Key,
     detail::Comparator< Key > Comparator = std::less< Key >,
     typename... Values >
    class Map
    {
       public:
        constexpr Map(std::array< Key, sizeof...(Values) > keys,
         Tuple< Values... > values,
         Comparator cmp):
          keys_(std::move(keys)),
          values_(std::move(values))
        {
            std::ranges::sort(keys_, std::forward< Comparator >(cmp));
        }

        template < typename F >
        constexpr auto visit_at(const Key & key, F f)
        {
            return visit_at_impl(idx(key), std::forward< F >(f));
        }

        template < typename... Args >
        constexpr auto call_at(const Key & key, Args &&... args)
        {
            return visit_at(key,
             [&](auto & f)
             {
                 return f(std::forward< Args >(args)...);
             });
        }

        constexpr size_t idx(const Key & key)
        {
            auto it = std::ranges::upper_bound(keys_, key);
            --it;
            Key * k = *(it) == key ? std::addressof(*it) : nullptr;
            assert(k != nullptr);
            size_t idx = k - keys_.begin();
            return idx;
        }

        template < size_t I >
        constexpr auto & at()
        {
            return get< I >(values_);
        }

       private:
        template < size_t I = 0, typename F, typename... Args >
        auto visit_at_impl(size_t idx, F f)
        {
            if constexpr (I < decltype(values_)::size())
            {
                switch (idx)
                {
                case I:
                    return f(get< I >(values_));
                default:
                    return visit_at_impl< I + 1, F >(idx, std::forward< F >(f));
                }
            }
        }

        std::array< Key, sizeof...(Values) > keys_;
        Tuple< Values... > values_;
    };

    template < typename Key,
     detail::Comparator< Key > Comparator = std::less< Key >,
     typename... Values >
    auto make_map(Tuple< Key, Values >... keyvals)
    {
        return make_map(Comparator{}, std::move(keyvals)...);
    }

    template < typename Key,
     detail::Comparator< Key > Comparator = std::less< Key >,
     typename... Values >
    auto make_map(Comparator cmp, Tuple< Key, Values >... keyvals)
    {
        std::array< Key, sizeof...(Values) > keys{ std::forward< Key >(get< 0 >(keyvals))... };
        Tuple< Values... > values{ std::forward< Values >(get< 1 >(keyvals))... };
        return Map{ std::move(keys), std::move(values), std::forward< Comparator >(cmp) };
    }

    namespace detail
    {
        template < typename First, std::same_as< First > Second, typename... Args >
        struct common_type
        {
            using type = common_type< Second, Args... >;
        };

        template < typename First, std::same_as< First > Second >
        struct common_type< First, Second >
        {
            using type = Second;
        };

        template < typename First, std::same_as< First > Second, typename... Args >
        using common_type_t = typename common_type< First, Second, Args... >::type;
    }

    template < typename Key, typename Comparator, typename... Values >
    Map(std::array< Key, sizeof...(Values) > keys, Tuple< Values... > values_, Comparator cmp)
     -> Map< Key, Comparator, Values... >;

    template < typename Key, typename Comparator, typename... Values >
    Map(Tuple< Key, Values >... keyvals, Comparator cmp = Comparator{})
     -> Map< Key, Comparator, Values... >;
}

#endif