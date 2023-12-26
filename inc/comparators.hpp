#ifndef STCT_COMPARATORS_HPP
#define STCT_COMPARATORS_HPP

#include <cstddef>
#include <functional>

namespace static_containers
{
    template < typename T, typename U >
    concept Comparator = requires(T x, const U & rhs, const U & lhs) {
        {
            x(rhs, lhs)
        } -> std::convertible_to< bool >;
    };

    template < typename T, typename Lhs, typename Rhs >
    concept TypeComparator = requires(T, Lhs, Rhs) {
        {
            T::template value< Lhs, Rhs >()
        } -> std::convertible_to< bool >;
    };
}

namespace static_containers::comparators::size_of
{
    template < Comparator< size_t > Comparator, Comparator CMP = Comparator{} >
    struct compare
    {
        template < typename T1, typename T2 >
        constexpr static bool value() noexcept
        {
            return CMP(sizeof(T1), sizeof(T2));
        }
    };

    using less = compare< std::less< size_t > >;
    using greater = compare< std::greater< size_t > >;
    using eq = compare< std::equal_to< size_t > >;
    using neq = compare< std::not_equal_to< size_t > >;
    using lesseq = compare< std::less_equal< size_t > >;
    using greatereq = compare< std::greater_equal< size_t > >;
}

namespace static_containers::comparators::align_of
{
    template < Comparator< size_t > Comparator, Comparator CMP = Comparator{} >
    struct compare
    {
        template < typename T1, typename T2 >
        constexpr static bool value() noexcept
        {
            return CMP(sizeof(T1), sizeof(T2));
        }
    };

    using less = compare< std::less< size_t > >;
    using greater = compare< std::greater< size_t > >;
    using eq = compare< std::equal_to< size_t > >;
    using neq = compare< std::not_equal_to< size_t > >;
    using lesseq = compare< std::less_equal< size_t > >;
    using greatereq = compare< std::greater_equal< size_t > >;
}

#endif