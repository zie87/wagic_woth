#ifndef JGE_PLATFORM_SDL_MUTEX_HPP
#define JGE_PLATFORM_SDL_MUTEX_HPP

#include "jge/thread/unique_lock.hpp"

#include <mutex>

namespace jge {

class mutex {
public:
    using scoped_lock = ::jge::unique_lock<mutex>;

    mutex() noexcept {}

    mutex(const mutex&)            = delete;
    mutex& operator=(const mutex&) = delete;

    inline void lock() { m_mutex.lock(); }
    inline void unlock() { m_mutex.unlock(); }

private:
    std::mutex m_mutex = {};
};

}  // namespace jge

#endif  // JGE_PLATFORM_SDL_MUTEX_HPP
