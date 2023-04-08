#ifndef JGE_PLATFORM_SDL_THIS_THREAD_HPP
#define JGE_PLATFORM_SDL_THIS_THREAD_HPP

#include "jge/time/duration.hpp"

namespace jge::this_thread {

inline void sleep(const ::jge::milliseconds& time) { std::this_thread::sleep_for(time); }

template <typename R, typename P>
inline void sleep(const ::jge::duration<R, P>& time) {
    ::jge::this_thread::sleep(::std::chrono::duration_cast<::jge::milliseconds>(time));
}
}  // namespace jge::this_thread

#endif  // JGE_PLATFORM_SDL_THIS_THREAD_HPP
