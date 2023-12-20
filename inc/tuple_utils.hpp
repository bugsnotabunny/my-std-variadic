#ifndef TUPLE_UTILS_HPP
#define TUPLE_UTILS_HPP

#include <cstddef>
#include <utility>

#include "tuple.hpp"
#include "concepts.hpp"

namespace static_containers
{

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

        template < size_t I >
        constexpr const auto & at() const
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

    template < size_t I, size_t BEGIN, size_t END, typename... Args >
    constexpr auto & get(TupleView< BEGIN, END, Args... > & rhs)
    {
        return rhs.template at< I >();
    }

    template < size_t I, size_t BEGIN, size_t END, typename... Args >
    constexpr const auto & get(const TupleView< BEGIN, END, Args... > & rhs)
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
            static constexpr auto unwrap(F f, Tpl tpl, Tpls... tpls, Args... args)
            {
                if constexpr (I < Tpl::size())
                {
                    auto next_arg = get< I >(std::forward< Tpl >(tpl));
                    using NextArgType = decltype(next_arg);
                    return Unwrapper< Args...,
                     NextArgType >::template unwrap< I + 1, F, Tpl, Tpls... >(std::forward< F >(f),
                     std::forward< Tpl >(tpl),
                     std::forward< Tpls >(tpls)...,
                     std::forward< Args >(args)...,
                     std::forward< NextArgType >(next_arg));
                }
                else if constexpr (sizeof...(Tpls) > 0)
                {
                    return Unwrapper< Args... >::unwrap< 0, F, Tpls... >(std::forward< F >(f),
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
        return detail::Unwrapper<>::unwrap(std::forward< F >(f), std::forward< Tpls >(tpls)...);
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