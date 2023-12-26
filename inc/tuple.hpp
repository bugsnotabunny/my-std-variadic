#ifndef STCT_TUPLE_HPP
#define STCT_TUPLE_HPP

#include <cstddef>
#include <utility>

#include "traits.hpp"

namespace static_containers
{
    namespace detail
    {
        template < typename... Args >
        struct TupleHead;

        template < typename T, typename... Args >
        struct TupleHead< T, Args... >
        {
            TupleHead(T v, Args... args):
              v(std::forward< T >(v)),
              next(std::forward< Args >(args)...)
            {}
            TupleHead(const TupleHead &) = default;
            TupleHead(TupleHead &&) = default;
            TupleHead & operator=(const TupleHead &) = default;
            TupleHead & operator=(TupleHead &&) = default;

            template < size_t I >
            constexpr auto & at() noexcept
            {
                if constexpr (I == 0)
                {
                    return v;
                }
                else
                {
                    return next.template at< I - 1 >();
                }
            }

            template < size_t I >
            constexpr const auto & at() const noexcept
            {
                if constexpr (I == 0)
                {
                    return v;
                }
                else
                {
                    return next.template at< I - 1 >();
                }
            }

            auto operator<=>(const TupleHead & rhs) const noexcept = default;

            T v;
            TupleHead< Args... > next;
        };

        template <>
        struct TupleHead<>
        {
            TupleHead() = default;

            TupleHead(const TupleHead &) = default;
            TupleHead(TupleHead &&) = default;
            TupleHead & operator=(const TupleHead &) = default;
            TupleHead & operator=(TupleHead &&) = default;

            template < size_t I >
            constexpr const auto & at() const noexcept
            {
                static_assert(I == -1, "Requested tuple index is out of bounds");
            }

            auto operator<=>(const TupleHead & rhs) const noexcept = default;
        };
    }

    template < size_t BEGIN, size_t END, typename... Args >
    class TupleView;

    /// Mostly same as std::tuple, but with some flavour on top: TupleView for complete or partial
    /// iteration
    template < typename... Args >
    class Tuple
    {
       public:
        using args_as_pack = pack< Args... >;

        Tuple(Args... args):
          head_(std::forward< Args >(args)...){};

        Tuple(const Tuple &) = default;
        Tuple(Tuple &&) = default;
        Tuple & operator=(const Tuple &) = default;
        Tuple & operator=(Tuple &&) = default;

        template < size_t I >
        constexpr auto & at()
        {
            return head_.template at< I >();
        }

        template < size_t I >
        constexpr const auto & at() const
        {
            return head_.template at< I >();
        }

        static constexpr size_t size() noexcept
        {
            return sizeof...(Args);
        }

        constexpr TupleView< 0, sizeof...(Args), Args... > view_full() noexcept
        {
            return { *this };
        }

        template < size_t BEGIN_IDX, size_t END_IDX >
        constexpr TupleView< BEGIN_IDX, END_IDX, Args... > view() noexcept
        {
            return view_full()
             .template shrink_front< BEGIN_IDX >()
             .template shrink_back< size() - END_IDX >();
        }

        constexpr auto operator<=>(const Tuple & rhs) const noexcept = default;

       private:
        detail::TupleHead< Args... > head_;
    };

    namespace detail
    {
        template < size_t I = 0, typename... Args >
        constexpr void assign_ref_tuple_impl(Tuple< Args &... > & lhs, Tuple< Args... > rhs)
        {
            if constexpr (I < sizeof...(Args))
            {
                get< I >(lhs) = get< I >(rhs);
                assign_ref_tuple_impl< I + 1 >(lhs, rhs);
            }
        }
    }

    template < typename... Args >
    class Tuple< Args &... >
    {
       public:
        constexpr Tuple(Args &... args):
          head_(args...){};

        Tuple(const Tuple &) = default;
        Tuple(Tuple &&) = default;
        Tuple & operator=(const Tuple &) = default;
        Tuple & operator=(Tuple &&) = default;

        constexpr Tuple & operator=(Tuple< Args... > & rhs)
        {
            detail::assign_ref_tuple_impl(*this, rhs);
            return *this;
        }

        template < size_t I >
        constexpr auto & at()
        {
            return head_.template at< I >();
        }

        template < size_t I >
        constexpr const auto & at() const
        {
            return head_.template at< I >();
        }

        static constexpr size_t size() noexcept
        {
            return sizeof...(Args);
        }

        constexpr TupleView< 0, sizeof...(Args), Args... > view_full() noexcept
        {
            return { *this };
        }

        template < size_t BEGIN_IDX, size_t END_IDX >
        constexpr TupleView< BEGIN_IDX, END_IDX, Args... > view() noexcept
        {
            return view_full()
             .template shrink_front< BEGIN_IDX >()
             .template shrink_back< size() - END_IDX >();
        }

        auto operator<=>(const Tuple & rhs) const noexcept = default;

       private:
        detail::TupleHead< Args &... > head_;
    };

    template < size_t I, typename... Args >
    constexpr auto & get(Tuple< Args... > & rhs)
    {
        return rhs.template at< I >();
    }

    template < size_t I, typename... Args >
    constexpr const auto & get(const Tuple< Args... > & rhs)
    {
        return rhs.template at< I >();
    }

    template < size_t I, typename... Args >
    constexpr auto && get(Tuple< Args... > && rhs)
    {
        return std::move(rhs.template at< I >());
    }

    template < typename... Args >
    constexpr Tuple< Args &... > tie(Args &... args) noexcept
    {
        return { args... };
    }

    constexpr auto make_tuple = []< typename... Args >(Args &&... args)
    {
        return Tuple{ std::forward< Args >(args)... };
    };
}

#endif