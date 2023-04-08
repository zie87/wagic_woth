#ifndef JGE_THREAD_UNIQUE_LOCK_HPP
#define JGE_THREAD_UNIQUE_LOCK_HPP

namespace jge {

template <typename MUTEX_T>
class unique_lock {
public:
    using mutex_type = MUTEX_T;

    unique_lock(mutex_type& mtx) noexcept : m_mutex(&mtx) { m_mutex->lock(); }
    ~unique_lock() { m_mutex->unlock(); }

private:
    mutex_type* m_mutex = nullptr;
};

}  // namespace jge

#endif  // JGE_THREAD_UNIQUE_LOCK_HPP
