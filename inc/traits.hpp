#include <cstddef>
namespace static_containers
{
    namespace detail
    {
        template < int DESIRED, int I, typename T, typename... Ts >
        struct ith_impl
        {
            using type = typename ith_impl< DESIRED, I + 1, Ts... >::type;
        };

        template < int DESIRED, typename T, typename... Ts >
        struct ith_impl< DESIRED, DESIRED - 1, T, Ts... >
        {
            using type = T;
        };
    }

    template < int I, typename T, typename... Ts >
    struct ith
    {
        using type = detail::ith_impl< I, 0, Ts... >::type;
    };

    namespace detail
    {
        template < typename TypeComparator >
        constexpr bool pack_sorted_impl() noexcept
        {
            return true;
        }

        template < typename TypeComparator, typename First >
        constexpr bool pack_sorted_impl() noexcept
        {
            return pack_sorted_impl< TypeComparator >();
        }

        template < typename TypeComparator, typename First, typename Second, typename... Other >
        constexpr bool pack_sorted_impl() noexcept
        {
            return !TypeComparator::template value< Second, First >() &&
                   pack_sorted_impl< TypeComparator, Second, Other... >();
        }
    }

    template < typename... Args >
    struct pack
    {
        template < size_t I >
        using type_at = ith< I, Args... >::type;

        template < typename TypeComparator >
        constexpr static bool is_sorted() noexcept
        {
            return detail::pack_sorted_impl< TypeComparator, Args... >();
        }
    };
}