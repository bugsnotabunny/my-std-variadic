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

        template < bool isconst, size_t I = 0, typename F, typename... Values >
        constexpr auto visit_at_impl(size_t idx,
         F f,
         std::conditional_t< isconst, const Tuple< Values... > &, Tuple< Values... > & > values)
        {
            if constexpr (I < sizeof...(Values) - 1)
            {
                switch (idx)
                {
                case I:
                    return f(get< I >(values));
                default:
                    return visit_at_impl< isconst, I + 1, F, Values... >(idx,
                     std::forward< F >(f),
                     values);
                }
            }
            else if constexpr (I == sizeof...(Values) - 1)
            {
                switch (idx)
                {
                case I:
                    return f(get< I >(values));
                default:
                    assert(false);
                }
            }
        }
    }

    template < typename Key,
     detail::Comparator< Key > Comparator = std::less< Key >,
     typename... Values >
    class Map
    {
       public:
        constexpr Map(std::array< Key, sizeof...(Values) > sortedkeys,
         Tuple< Values... > values,
         Comparator cmp) noexcept:
          keys_(std::move(sortedkeys)),
          values_(std::move(values))
        {
            std::ranges::sort(keys_, std::forward< Comparator >(cmp));
            assert(std::ranges::is_sorted(sortedkeys));
        }

        template < typename F >
        constexpr auto visit_at(const Key & key, F f)
        {
            return detail::visit_at_impl< false, 0, F, Values... >(idx(key),
             std::forward< F >(f),
             values_);
        }

        template < typename F >
        constexpr auto visit_at(const Key & key, F f) const
        {
            return detail::visit_at_impl< true, 0, F, Values... >(idx(key),
             std::forward< F >(f),
             values_);
        }

        constexpr size_t idx(const Key & key) const
        {
            auto it = std::ranges::upper_bound(keys_, key);
            --it;
            const Key * k = *(it) == key ? std::addressof(*it) : nullptr;
            assert(k != nullptr);
            size_t idx = k - keys_.begin();
            return idx;
        }

        template < size_t I >
        constexpr auto & at()
        {
            return get< I >(values_);
        }

        template < size_t I >
        constexpr const auto & at() const
        {
            return get< I >(values_);
        }

       private:
        std::array< Key, sizeof...(Values) > keys_;
        Tuple< Values... > values_;
    };

    template < typename Key,
     detail::Comparator< Key > Comparator = std::less< Key >,
     typename... Values >
    constexpr auto make_map(Comparator cmp, Tuple< Key, Values >... keyvals) noexcept
    {
        return Map{ std::array{ std::forward< Key >(get< 0 >(keyvals))... },
            Tuple{ std::forward< Values >(get< 1 >(keyvals))... },
            std::forward< Comparator >(cmp) };
    }

    template < typename Key,
     detail::Comparator< Key > Comparator = std::less< Key >,
     typename... Values >
    constexpr auto make_map(Tuple< Key, Values >... keyvals) noexcept
    {
        return make_map(Comparator{}, std::forward< Tuple< Key, Values > >(keyvals)...);
    }
}

namespace static_containers::visitors
{
    template < typename Key, typename... Args >
    constexpr auto call_at(auto & map, const Key & key, Args... args)
    {
        return map.visit_at(key,
         [&](auto & f)
         {
             return f(std::forward< Args >(args)...);
         });
    }

    template < typename Key, typename... Args >
    constexpr auto call_at(const auto & map, const Key & key, Args... args)
    {
        return map.visit_at(key,
         [&](const auto & f)
         {
             return f(std::forward< Args >(args)...);
         });
    }

    template < typename Key >
    constexpr auto incr_at(auto & map, const Key & key)
    {
        return map.visit_at(key,
         [&](auto & i)
         {
             ++i;
         });
    }

    template < typename Key >
    constexpr auto decr_at(auto & map, const Key & key)
    {
        return map.visit_at(key,
         [&](auto & i)
         {
             --i;
         });
    }

}

#endif