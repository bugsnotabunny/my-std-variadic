#ifndef TEST_DROP_LOGGER_HPP
#define TEST_DROP_LOGGER_HPP

#include <cassert>
#include <cstddef>
#include <utility>
#include <functional>

namespace static_containers::testing
{
    class DropLogger
    {
       public:
        DropLogger(size_t & cnt):
          cnt_(cnt)
        {}

        DropLogger(const DropLogger &) = default;

        DropLogger(DropLogger && rhs):
          cnt_(rhs.cnt_),
          moved_(rhs.moved_),
          destroyed_(rhs.destroyed_)
        {
            rhs.moved_ = true;
        }

        DropLogger & operator=(const DropLogger &) = default;

        DropLogger & operator=(DropLogger && rhs)
        {
            cnt_ = rhs.cnt_;
            moved_ = rhs.moved_;
            destroyed_ = rhs.destroyed_;
            rhs.moved_ = true;
            return *this;
        }

        ~DropLogger()
        {
            assert(!destroyed_);
            destroyed_ = true;
            if (!moved_)
            {
                ++cnt_;
            }
        }

       private:
        std::reference_wrapper< size_t > cnt_;
        bool moved_ = false;
        bool destroyed_ = false;
    };
}

#endif