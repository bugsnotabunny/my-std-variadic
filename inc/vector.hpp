#ifndef STATIC_VECTOR_HPP
#define STATIC_VECTOR_HPP

#include <algorithm>
#include <bits/iterator_concepts.h>
#include <bits/ranges_base.h>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace static_containers
{
    /// Vector with compile-time known capacity
    template < typename T, size_t N >
    class Vector
    {
       public:
        using ValueType = T;
        using SizeType = size_t;
        using DifferenceType = std::ptrdiff_t;
        using Reference = T &;
        using ConstReference = const T &;
        using Pointer = T *;
        using ConstPointer = const T *;
        using Iterator = T *;
        using ConstIterator = const T *;
        using ReverseIterator = std::reverse_iterator< T * >;
        using ConstReverseIterator = std::reverse_iterator< T * >;

        constexpr Vector() noexcept:
          size_(0)
        {}

        constexpr Vector(std::initializer_list< T > init):
          Vector()
        {
            assign(init);
        }

        constexpr Vector(SizeType count, const T & value):
          Vector()
        {
            assign(count, value);
        }

        constexpr Vector(SizeType count):
          Vector()
        {
            assign(count, T{});
        }

        template < std::input_iterator It >
        constexpr Vector(It begin, It end):
          Vector()
        {
            assign(begin, end);
        }

        constexpr Vector(const Vector & rhs):
          size_(0)
        {
            for (const auto & item: rhs)
            {
                push_back(item);
            }
        }

        constexpr Vector(Vector && rhs):
          size_(rhs.size_)
        {
            assign(rhs.begin(), rhs.end());
            rhs.size_ = 0;
        }

        constexpr Vector & operator=(std::initializer_list< T > init)
        {
            Vector tmp{ std::move(init) };
            std::swap(*this, tmp);
            return *this;
        }

        constexpr Vector & operator=(const Vector & rhs)
        {
            Vector tmp{ rhs };
            std::swap(*this, tmp);
            return *this;
        }

        constexpr Vector & operator=(Vector && rhs)
        {
            std::swap(*this, rhs);
            return *this;
        }

        ~Vector()
        {
            std::destroy(begin(), end());
        }

        constexpr void assign(SizeType count, const T & value)
        {
            while (size_ < count)
            {
                push_back(value);
            }
        }

        template < std::input_iterator It >
        constexpr void assign(It begin, It end)
        {
            for (; begin != end; ++begin)
            {
                push_back(std::move(*begin));
            }
        }

        constexpr void assign(std::initializer_list< T > init)
        {
            for (auto && elem: init)
            {
                push_back(std::move(elem));
            }
        }

        constexpr Reference at(SizeType pos)
        {
            const auto & self = *this;
            return const_cast< Reference >(self.at(pos));
        }

        constexpr ConstReference at(SizeType pos) const
        {
            if (pos >= size_)
            {
                throw std::out_of_range{ "static vector index is out of range" };
            }
            return operator[](pos);
        }

        constexpr Reference operator[](SizeType pos) noexcept
        {
            const auto & self = *this;
            return const_cast< Reference >(self[pos]);
        }

        constexpr ConstReference operator[](SizeType pos) const noexcept
        {
            assert(pos < size_);
            return *(cbegin() + pos);
        }

        constexpr Reference front() noexcept
        {
            const auto & self = *this;
            return const_cast< Reference >(self.front());
        }

        constexpr ConstReference front() const noexcept
        {
            assert(!empty());
            return operator[](0);
        }

        constexpr Reference back() noexcept
        {
            const auto & self = *this;
            return const_cast< Reference >(self.back());
        }

        constexpr ConstReference back() const noexcept
        {
            assert(!empty());
            return operator[](size_ - 1);
        }

        constexpr Pointer data() noexcept
        {
            const auto & self = *this;
            return const_cast< Pointer >(self.data());
        }

        constexpr ConstPointer data() const noexcept
        {
            return cbegin();
        }

        constexpr Iterator begin() noexcept
        {
            return const_cast< Iterator >(cbegin());
        }

        constexpr ConstIterator begin() const noexcept
        {
            return cbegin();
        }

        constexpr ConstIterator cbegin() const noexcept
        {
            return std::launder(reinterpret_cast< const T * >(std::addressof(storage_[0])));
        }

        constexpr Iterator end() noexcept
        {
            return const_cast< Iterator >(cend());
        }

        constexpr ConstIterator end() const noexcept
        {
            return cend();
        }

        constexpr ConstIterator cend() const noexcept
        {
            return cbegin() + size_;
        }

        constexpr ReverseIterator rbegin() noexcept
        {
            return end();
        }

        constexpr ConstReverseIterator rbegin() const noexcept
        {
            return end();
        }

        constexpr ConstReverseIterator crbegin() const noexcept
        {
            return cend();
        }

        constexpr ReverseIterator rend() noexcept
        {
            return begin();
        }

        constexpr ConstReverseIterator rend() const noexcept
        {
            return begin();
        }

        constexpr ConstReverseIterator crend() const noexcept
        {
            return cbegin();
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return size_ == 0;
        }

        constexpr SizeType size() const noexcept
        {
            return size_;
        }

        static constexpr SizeType capacity() noexcept
        {
            return max_size();
        }

        static constexpr SizeType max_size() noexcept
        {
            return N;
        }

        template < typename... Args >
        constexpr Reference emplace_back(Args &&... args)
        {
            return *emplace(cend(), std::forward< Args >(args)...);
        }

        constexpr void push_back(const T & value)
        {
            emplace_back(value);
        }

        constexpr void push_back(T && value)
        {
            emplace_back(std::move(value));
        }

        constexpr void pop_back()
        {
            assert(!empty());
            std::destroy_at(std::addressof(operator[](size_ - 1)));
            --size_;
        }

        constexpr void clear() noexcept
        {
            std::destroy(begin(), end());
            size_ = 0;
        }

        constexpr void resize(SizeType count)
        {
            resize(count, ValueType{});
        }

        constexpr void resize(SizeType count, const ValueType & value)
        {
            if (size_ == count)
            {
                return;
            }
            else if (size_ > count)
            {
                std::destroy(begin() + count, end());
                size_ = count;
                return;
            }
            while (size_ < count)
            {
                emplace_back(value);
            }
        }

        constexpr Iterator insert(ConstIterator pos, const T & value)
        {
            T to_insert{ value };
            return insert(pos, std::move(to_insert));
        }

        constexpr Iterator insert(ConstIterator pos, T && value) noexcept
        {
            return emplace(pos, std::move(value));
        }

        constexpr Iterator insert(ConstIterator pos, SizeType count, const T & value) noexcept
        {
            std::vector< T > to_insert(count, value);
            return insert(pos, to_insert.begin(), to_insert.end());
        }

        template < std::input_iterator It >
        constexpr Iterator insert(ConstIterator pos, It first, It last) noexcept
        {
            SizeType amount_to_insert = std::distance(first, last);

            assert(size_ + amount_to_insert <= capacity());

            shift_elems(amount_to_insert, end() - 1, std::distance(pos, cend()));
            auto pos_idx = pos - begin();
            for (SizeType i = pos_idx; i != pos_idx + amount_to_insert; ++first, ++i)
            {
                construct_at(i, std::move(*first));
            }
            size_ += amount_to_insert;
            return begin() + pos_idx;
        }

        constexpr Iterator insert(ConstIterator pos, std::initializer_list< T > init) noexcept
        {
            return insert(pos, init.begin(), init.end());
        }

        template < typename... Args >
        constexpr Iterator emplace(ConstIterator pos, Args &&... args)
        {
            assert(size_ + 1 <= capacity());
            shift_elems(1, end() - 1, std::distance(pos, cend()));
            auto pos_idx = pos - begin();
            construct_at(pos_idx, std::forward< Args >(args)...);
            ++size_;
            return begin() + pos_idx;
        }

        constexpr Iterator erase(ConstIterator pos)
        {
            auto pos_mut = begin() + (pos - begin());
            std::destroy_at(pos_mut);
            shift_elems(-1, pos_mut + 1, std::distance(pos_mut, end()));
            --size_;
            return pos_mut;
        }

        constexpr Iterator erase(ConstIterator first, ConstIterator last)
        {
            auto first_mut = begin() + (first - begin());
            auto last_mut = begin() + (last - begin());
            auto count = std::distance(first, last);
            std::destroy(first_mut, last_mut);
            shift_elems(-count, last_mut, std::distance(first_mut, end()));
            size_ -= count;
            return first_mut;
        }

       private:
        template < typename... Args >
        constexpr void construct_at(SizeType pos, Args... args)
        {
            assert(pos < N);
            ::new (std::addressof(storage_[pos])) T(std::forward< Args >(args)...);
        }

        constexpr static void shift_elems(DifferenceType shift, Iterator from, SizeType count)
        {
            auto sign = [](auto num)
            {
                return (num > 0) - (num < 0);
            };
            DifferenceType inc = -sign(shift);
            Iterator to = from + shift;
            for (SizeType i = 0; i < count; from += inc, to += inc, ++i)
            {
                *to = std::move(*from);
            }
        }

        SizeType size_;
        std::aligned_storage_t< sizeof(T), alignof(T) > storage_[N];
    };

    template < typename T, size_t N >
    constexpr bool operator==(const Vector< T, N > & lhs, const Vector< T, N > & rhs)
    {
        bool result = lhs.size() == rhs.size();
        auto lit = lhs.begin();
        auto rit = rhs.begin();
        for (; result && lit != lhs.end(); ++lit, ++rit)
        {
            result = result && *lit == *rit;
        }
        return result;
    }

    template < typename T, size_t N >
    constexpr auto operator<=>(const Vector< T, N > & lhs, const Vector< T, N > & rhs)
    {
        return std::lexicographical_compare_three_way(lhs.begin(),
         lhs.end(),
         rhs.begin(),
         rhs.end());
    }

    template < typename T, size_t N, typename U >
    constexpr typename Vector< T, N >::SizeType erase(Vector< T, N > & c, const U & value)
    {
        auto it = std::remove(c.begin(), c.end(), value);
        auto count = c.end() - it;
        c.erase(it, c.end());
        return count;
    }

    template < typename T, size_t N, typename Pred >
    constexpr typename Vector< T, N >::SizeType erase_if(Vector< T, N > & c, Pred pred)
    {
        auto it = std::remove_if(c.begin(), c.end(), pred);
        auto r = c.end() - it;
        c.erase(it, c.end());
        return r;
    }
}

#endif