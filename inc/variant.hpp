#ifndef STCT_VARIANT_HPP
#define STCT_VARIANT_HPP

#include <cstddef>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

namespace static_containers
{
    namespace detail
    {
        template < typename T, typename... Args >
        class VariadicUnion
        {
           public:
            VariadicUnion():
              node_()
            {}

            template < size_t I >
            constexpr auto & at() noexcept
            {
                if constexpr (I == 0)
                {
                    return node_.val;
                }
                else
                {
                    return node_.next.template at< I - 1 >();
                }
            }

            template < typename U, size_t I = 0 >
            static constexpr size_t find_where() noexcept
            {
                if constexpr (std::is_same_v< U, T >)
                {
                    return I;
                }
                else
                {
                    return VariadicUnion< Args... >::template find_where< U, I + 1 >();
                }
            }

            template < typename U, size_t I = 0 >
            constexpr size_t set(U val)
            {
                if constexpr (std::is_same_v< T, U >)
                {
                    node_.val = std::move(val);
                    return I;
                }
                else
                {
                    return node_.next.template set< U, I + 1 >(std::move(val));
                }
            }

           private:
            union Node
            {
                Node()
                {
                    std::memset(this, 0, sizeof(Node));
                }

                ~Node()
                {}

                T val;
                VariadicUnion< Args... > next;
            };
            Node node_;
        };

        template < typename T >
        class VariadicUnion< T >
        {
           public:
            VariadicUnion() = default;

            template < size_t I >
            constexpr auto & at() noexcept
            {
                static_assert(I == 0, "Requested variant index is out of bounds");
                return val_;
            }

            template < typename U, size_t I = 0 >
            static constexpr size_t find_where() noexcept
            {
                static_assert(std::is_same_v< T, U >, "Requested variant type is not present");
                return I;
            }

            template < typename U, size_t I = 0 >
            constexpr size_t set(U val)
            {
                static_assert(std::is_same_v< T, U >, "Requested variant type is not present");
                val_ = std::move(val);
                return I;
            }

           private:
            T val_;
        };
    }

    struct Emptiness
    {};

    namespace detail
    {
        template < typename T, typename... Args >
        T construct_first_defaulted()
        {
            return T{};
        }
    }

    /// Same as std::variant for now. Maybe worse.
    template < typename... Args >
    class Variant
    {
        using UnionT = detail::VariadicUnion< Args... >;

       public:
        constexpr Variant():
          selected_(0),
          union_()
        {
            union_.set(detail::construct_first_defaulted< Args... >());
        }

        template < typename T >
        Variant(T val):
          selected_(UnionT::template find_where< T >()),
          union_()
        {
            union_.set(std::move(val));
        }

        Variant(const Variant &) = default;
        Variant(Variant &&) = default;
        Variant & operator=(const Variant &) = default;
        Variant & operator=(Variant &&) = default;

        ~Variant()
        {
            visit(*this,
             [](auto & elem)
             {
                 std::destroy_at(std::addressof(elem));
             });
        }

        template < typename T >
        Variant & operator=(T val)
        {
            selected_ = union_.template set(std::move(val));
            return *this;
        }

        constexpr size_t index() const noexcept
        {
            return selected_;
        }

        static constexpr size_t alternatives() noexcept
        {
            return sizeof...(Args);
        }

        static_assert(alternatives() < std::numeric_limits< unsigned char >::max());

        template < typename U >
        constexpr bool holds() const noexcept
        {
            return selected_ == UnionT::template find_where< U >();
        }

        template < typename U >
        constexpr auto & at_t()
        {
            if (!holds< U >())
            {
                throw std::bad_variant_access{};
            }
            constexpr size_t I = UnionT::template find_where< U >();
            return union_.template at< I >();
        }

        template < size_t I >
        constexpr auto & at()
        {
            if (selected_ != I)
            {
                throw std::bad_variant_access{};
            }
            return union_.template at< I >();
        }

       private:
        unsigned char selected_;
        UnionT union_;
    };

    template < typename T, typename... Args >
    auto & get(Variant< Args... > & var)
    {
        return var.template at_t< T >();
    }

    template < size_t I, typename... Args >
    auto & get(Variant< Args... > & var)
    {
        return var.template at< I >();
    }

    namespace detail
    {
        template < size_t I = 0, typename F, typename... Args >
        auto visit_impl(Variant< Args... > & var, F & f)
        {
            if constexpr (I < Variant< Args... >::alternatives())
            {
                switch (var.index())
                {
                case I:
                    return f(get< I >(var));
                default:
                    return visit_impl< I + 1, F, Args... >(var, f);
                }
            }
            else
            {
                throw std::bad_variant_access{};
                // a trick to outwit return type deduction
                return visit_impl< I - 1, F, Args... >(var, f);
            }
        }
    }

    template < typename F, typename... Args >
    auto visit(Variant< Args... > & var, F f)
    {
        return detail::visit_impl(var, f);
    }
}

#endif