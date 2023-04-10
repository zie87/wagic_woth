#ifndef JGE_THREAD_THREAD_HPP
#define JGE_THREAD_THREAD_HPP

#if defined(PSP)
#include "jge/platform/psp/thread.hpp"
#include "jge/platform/psp/this_thread.hpp"
#else
#include "jge/platform/sdl/thread.hpp"
#include "jge/platform/sdl/this_thread.hpp"
#endif

#endif  // JGE_THREAD_THREAD_HPP
