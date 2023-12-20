#ifndef STCT_COMPARATORS_HPP
#define STCT_COMPARATORS_HPP

#include <cstddef>
#include <functional>

#include "concepts.hpp"

namespace static_containers::comparators::size_of
{
    template < typename T1,
     typename T2,
     Comparator< size_t > Comparator,
     Comparator CMP = Comparator{} >
    struct compare
    {
        constexpr static bool value = CMP(sizeof(T1), sizeof(T2));
    };

    template < typename T1,
     typename T2,
     Comparator< size_t > Comparator,
     Comparator CMP = Comparator{} >
    constexpr static bool compare_v = compare< T1, T2, Comparator, CMP >::value;

    template < typename T1, typename T2 >
    using less = compare< T1, T2, std::less< size_t > >;

    template < typename T1, typename T2 >
    constexpr static bool less_v = less< T1, T2 >::value;

    template < typename T1, typename T2 >
    using greater = compare< T1, T2, std::greater< size_t > >;

    template < typename T1, typename T2 >
    constexpr static bool grearer_v = greater< T1, T2 >::value;

    template < typename T1, typename T2 >
    using eq = compare< T1, T2, std::equal_to< size_t > >;

    template < typename T1, typename T2 >
    constexpr static bool eq_v = eq< T1, T2 >::value;

    template < typename T1, typename T2 >
    using neq = compare< T1, T2, std::not_equal_to< size_t > >;

    template < typename T1, typename T2 >
    constexpr static bool neq_v = neq< T1, T2 >::value;

    template < typename T1, typename T2 >
    using lesseq = compare< T1, T2, std::less_equal< size_t > >;

    template < typename T1, typename T2 >
    constexpr static bool lesseq_v = lesseq< T1, T2 >::value;

    template < typename T1, typename T2 >
    using greatereq = compare< T1, T2, std::greater_equal< size_t > >;

    template < typename T1, typename T2 >
    constexpr static bool greatereq_v = greatereq< T1, T2 >::value;
}

#endif