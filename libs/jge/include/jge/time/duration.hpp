#ifndef JGE_TIME_DURATION_HPP
#define JGE_TIME_DURATION_HPP

#include <chrono>

namespace jge {

template <typename R, typename P = std::ratio<1>>
using duration = std::chrono::duration<R, P>;

using nanoseconds  = std::chrono::nanoseconds;
using microseconds = std::chrono::microseconds;
using milliseconds = std::chrono::milliseconds;
using seconds      = std::chrono::seconds;
using minutes      = std::chrono::minutes;
using hours        = std::chrono::hours;

}  // namespace jge

#endif  // JGE_TIME_DURATION_HPP
