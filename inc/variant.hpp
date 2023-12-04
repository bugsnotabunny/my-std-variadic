
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

namespace mystd
{
    namespace detail
    {
        template < typename T, typename... Args >
        class VariadicUnion
        {
           public:
            VariadicUnion(T val):
              node_(std::move(val))
            {}

            template < size_t I >
            constexpr auto & at() noexcept
            {
                static_assert(I < sizeof...(Args) + 1);
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
                T val;
                VariadicUnion< Args... > next;
            };
            Node node_;
        };

        template < typename T >
        class VariadicUnion< T >
        {
           public:
            VariadicUnion(T val):
              val_(std::move(val))
            {}

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

    struct VariantEmpty
    {};

    template < typename... Args >
    class Variant
    {
       public:
        constexpr Variant():
          union_(VariantEmpty{})
        {}

        template < typename T >
        Variant(T val):
          union_(VariantEmpty{})
        {
            union_.set(std::move(val));
        }

        Variant(const Variant &) = default;
        Variant(Variant &&) = default;
        Variant & operator=(const Variant &) = default;
        Variant & operator=(Variant &&) = default;

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

        template < typename U >
        constexpr bool holds() const noexcept
        {
            return selected_ == decltype(union_)::template find_where< U >();
        }

        template < typename U >
        constexpr auto & at_t()
        {
            if (!holds< U >())
            {
                throw std::bad_variant_access{};
            }
            constexpr size_t I = decltype(union_)::template find_where< U >();
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
        size_t selected_;
        detail::VariadicUnion< VariantEmpty, Args... > union_;
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
}