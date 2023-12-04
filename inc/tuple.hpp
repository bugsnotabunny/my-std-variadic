#ifndef TUPLE_HPP
#define TUPLE_HPP

#include <cstddef>
#include <utility>

namespace mystd
{
    namespace detail
    {
        template < typename T, typename... Args >
        struct TupleHead
        {
            TupleHead(T v, Args... args):
              v(std::move(v)),
              next(args...)
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

            auto operator<=>(const TupleHead & rhs) const noexcept = default;

            T v;
            TupleHead< Args... > next;
        };

        template < typename T >
        struct TupleHead< T >
        {
            explicit TupleHead(T v):
              v(std::move(v))
            {}

            TupleHead(const TupleHead &) = default;
            TupleHead(TupleHead &&) = default;
            TupleHead & operator=(const TupleHead &) = default;
            TupleHead & operator=(TupleHead &&) = default;

            template < size_t I >
            constexpr auto & at()
            {
                static_assert(I == 0, "Requested tuple index is out of bounds");
                return v;
            }

            auto operator<=>(const TupleHead & rhs) const noexcept = default;

            T v;
        };
    }

    template < typename... Args >
    class Tuple
    {
       public:
        Tuple(Args... args):
          head_(args...){};

        Tuple(const Tuple &) = default;
        Tuple(Tuple &&) = default;
        Tuple & operator=(const Tuple &) = default;
        Tuple & operator=(Tuple &&) = default;

        template < size_t I >
        auto & at()
        {
            return head_.template at< I >();
        }

        constexpr size_t size() noexcept
        {
            return sizeof...(Args);
        }

        auto operator<=>(const Tuple & rhs) const noexcept = default;

       private:
        detail::TupleHead< Args... > head_;
    };

    namespace detail
    {
        template < typename T, typename... Args >
        struct TupleHead< T &, Args... >
        {
            TupleHead(T & v, Args... args):
              v(v),
              next(args...)
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

            auto operator<=>(const TupleHead & rhs) const noexcept = default;

            T & v;
            TupleHead< Args... > next;
        };

        template < typename T >
        struct TupleHead< T & >
        {
            explicit TupleHead(T & v):
              v(v)
            {}

            TupleHead(const TupleHead &) = default;
            TupleHead(TupleHead &&) = default;
            TupleHead & operator=(const TupleHead &) = default;
            TupleHead & operator=(TupleHead &&) = default;

            template < size_t I >
            constexpr auto & at()
            {
                static_assert(I == 0, "Requested tuple index is out of bounds");
                return v;
            }

            auto operator<=>(const TupleHead & rhs) const noexcept = default;

            T & v;
        };

        template < size_t I = 0, typename... Args >
        void assign_ref_tuple_impl(Tuple< Args &... > & lhs, Tuple< Args... > rhs)
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
        Tuple(Args &... args):
          head_(args...){};

        Tuple(const Tuple &) = default;
        Tuple(Tuple &&) = default;
        Tuple & operator=(const Tuple &) = default;
        Tuple & operator=(Tuple &&) = default;

        Tuple & operator=(Tuple< Args... > & rhs)
        {
            detail::assign_ref_tuple_impl(*this, rhs);
            return *this;
        }

        template < size_t I >
        auto & at()
        {
            return head_.template at< I >();
        }

        constexpr size_t size() noexcept
        {
            return sizeof...(Args);
        }
        auto operator<=>(const Tuple & rhs) const noexcept = default;

       private:
        detail::TupleHead< Args &... > head_;
    };

    template < size_t I, typename... Args >
    auto & get(Tuple< Args... > & rhs)
    {
        return rhs.template at< I >();
    }

    template < typename... Args >
    Tuple< Args &... > tie(Args &... args) noexcept
    {
        return { args... };
    }

    template < size_t I = 0, typename F, typename... Args >
    void for_each(Tuple< Args... > & tuple, F f)
    {
        if constexpr (I < sizeof...(Args))
        {
            f(get< I >(tuple));
            for_each< I + 1, F, Args... >(tuple, f);
        }
    }
}

#endif