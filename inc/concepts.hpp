#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <concepts>
#include <utility>

namespace static_containers
{
    template < typename T, typename U >
    concept Comparator = requires(T x, const U & rhs, const U & lhs) {
        {
            x(rhs, lhs)
        } -> std::convertible_to< bool >;
    };

    template < typename T, typename U1, typename U2 >
    concept TypeComparator = requires(T) {
        {
            T::value
        } -> std::convertible_to< bool >;
    };

    template < typename T >
    concept TupleLike = requires(T x) {
        get< 0 >(x);
        get< 0 >(std::move(x));
        get< 0 >(std::forward< T >(x));
        T::size();
    };
}

#endif