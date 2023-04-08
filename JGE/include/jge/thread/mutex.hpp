#ifndef JGE_THREAD_MUTEX_HPP
#define JGE_THREAD_MUTEX_HPP

#include "jge/thread/unique_lock.hpp"

#if defined(PSP)
#include "jge/platform/psp/mutex.hpp"
#else
#include "jge/platform/sdl/mutex.hpp"
#endif

#endif  // JGE_THREAD_MUTEX_HPP
