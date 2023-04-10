#ifndef JGE_PLATFORM_PSP_MUTEX_HPP
#define JGE_PLATFORM_PSP_MUTEX_HPP

#include "jge/thread/unique_lock.hpp"

#include "JLogger.h"

#include "pspthreadman.h"

namespace jge {

class mutex {
public:
    using scoped_lock = ::jge::unique_lock<mutex>;

    mutex() noexcept : m_id{} { m_id = ::sceKernelCreateSema("Unnamed", 0, 1, 1, 0); }
    ~mutex() noexcept { ::sceKernelDeleteSema(m_id); }

    mutex(const mutex&)            = delete;
    mutex& operator=(const mutex&) = delete;

    inline void lock() noexcept {
        const int result = ::sceKernelWaitSema(m_id, 1, 0);
        if (result < 0) {
            LOG("Semaphore error on lock acquire, mutex id: ");
            LOG((char*)m_id);
        }
    }

    inline void unlock() {
        const int result = ::sceKernelSignalSema(m_id, 1);
        if (result < 0) {
            LOG("Semaphore error on lock release, mutex id: ");
            LOG((char*)m_id);
        }
    }

private:
    int m_id = {};
};

}  // namespace jge

#endif  // JGE_PLATFORM_SDL_THREAD_HPP
