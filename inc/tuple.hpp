#ifndef STCT_TUPLE_HPP
#define STCT_TUPLE_HPP

#include <cstddef>
#include <utility>

namespace static_containers
{
    namespace detail
    {
        template < typename T, typename... Args >
        struct TupleHead
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

            auto operator<=>(const TupleHead & rhs) const noexcept = default;

            T v;
            TupleHead< Args... > next;
        };

        template < typename T >
        struct TupleHead< T >
        {
            explicit TupleHead(T v):
              v(std::forward< T >(v))
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

    template < size_t BEGIN, size_t END, typename... Args >
    class TupleView;

    /// Mostly same as std::tuple, but with some flavour on top: TupleView for complete or partial
    /// iteration
    template < typename... Args >
    class Tuple
    {
       public:
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

    template < size_t BEGIN, size_t END, typename... Args >
    class TupleView
    {
        static_assert(BEGIN != END);

       public:
        constexpr TupleView(Tuple< Args... > & tuple):
          tuple_(tuple)
        {}

        TupleView(const TupleView &) = default;
        TupleView(TupleView &&) = default;
        TupleView & operator=(const TupleView &) = default;
        TupleView & operator=(TupleView &&) = default;

        template < size_t N = 1 >
        constexpr TupleView< BEGIN + N, END, Args... > shrink_front()
        {
            return { tuple_ };
        }

        template < size_t N = 1 >
        constexpr TupleView< BEGIN, END - N, Args... > shrink_back()
        {
            return { tuple_ };
        }

        template < size_t I >
        constexpr auto & at()
        {
            static_assert(BEGIN + I < END);
            return get< BEGIN + I >(tuple_);
        }

        static constexpr size_t size()
        {
            return END - BEGIN;
        }

       private:
        Tuple< Args... > & tuple_;
    };

    template < typename T >
    concept TupleLike = requires(T x) {
        get< 0 >(x);
        get< 0 >(std::move(x));
        get< 0 >(std::forward< T >(x));
        T::size();
    };

    template < size_t I, typename... Args >
    constexpr auto & get(Tuple< Args... > & rhs)
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

    template < size_t I, size_t BEGIN, size_t END, typename... Args >
    constexpr auto & get(TupleView< BEGIN, END, Args... > & rhs)
    {
        return rhs.template at< I >();
    }

    template < size_t I, size_t BEGIN, size_t END, typename... Args >
    constexpr auto && get(TupleView< BEGIN, END, Args... > && rhs)
    {
        return std::move(rhs.template at< I >());
    }

    template < size_t I = 0, typename F, size_t BEGIN, size_t END, typename... Args >
    void for_each(TupleView< BEGIN, END, Args... > view, F f)
    {
        if constexpr (I < view.size())
        {
            f(get< I >(view));
            for_each< I + 1, F, BEGIN, END, Args... >(view, f);
        }
    }

    namespace detail
    {
        template < typename... Args >
        struct Unwrapper
        {
            template < size_t I = 0, typename F, TupleLike Tpl, TupleLike... Tpls >
            static constexpr auto unwrap_one(F f, Tpl tpl, Tpls... tpls, Args... args)
            {
                if constexpr (I < Tpl::size())
                {
                    auto next_arg = get< I >(std::forward< Tpl >(tpl));
                    using NextArgType = decltype(next_arg);
                    return Unwrapper< Args..., NextArgType >::
                     template unwrap_one< I + 1, F, Tpl, Tpls... >(std::forward< F >(f),
                      std::forward< Tpl >(tpl),
                      std::forward< Tpls >(tpls)...,
                      std::forward< Args >(args)...,
                      std::forward< NextArgType >(next_arg));
                }
                else if constexpr (sizeof...(Tpls) > 0)
                {
                    return Unwrapper< Args... >::unwrap_one< 0, F, Tpls... >(std::forward< F >(f),
                     std::forward< Tpls >(tpls)...,
                     std::forward< Args >(args)...);
                }
                else
                {
                    return f(std::forward< Args >(args)...);
                }
            }
        };
    }

    template < typename F, TupleLike... Tpls >
    constexpr auto unwrap_then_do(F f, Tpls... tpls)
    {
        return detail::Unwrapper<>::unwrap_one(std::forward< F >(f), std::forward< Tpls >(tpls)...);
    }

    template < TupleLike... Tpls >
    constexpr auto concat(Tpls... tpls)
    {
        return unwrap_then_do(
         [](auto &&... args)
         {
             return Tuple{ std::forward< decltype(args) >(args)... };
         },
         std::forward< Tpls >(tpls)...);
    }
}

#endif