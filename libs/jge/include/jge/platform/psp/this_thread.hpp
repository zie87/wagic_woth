#ifndef JGE_PLATFORM_PSP_THIS_THREAD_HPP
#define JGE_PLATFORM_PSP_THIS_THREAD_HPP

#include "jge/time/duration.hpp"

namespace jge {
namespace this_thread {

inline void sleep(const ::jge::microseconds& time) { ::sceKernelDelayThread(time.count()); }

template <typename R, typename P>
inline void sleep(const ::jge::duration<R, P>& time) {
    ::jge::this_thread::sleep(::std::chrono::duration_cast<::jge::microseconds>(time));
}
}  // namespace this_thread
}  // namespace jge

#endif  // JGE_PLATFORM_PSP_THIS_THREAD_HPP
