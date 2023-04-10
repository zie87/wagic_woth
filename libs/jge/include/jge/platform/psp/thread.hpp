#ifndef JGE_PLATFORM_PSP_THREAD_HPP
#define JGE_PLATFORM_PSP_THREAD_HPP

#include "jge/memory.hpp"

#include "JLogger.h"

#include "pspthreadman.h"

#include <functional>

namespace jge {
/**
** Emulating boost::thread configuration glue, with some shortcuts
** This detail namespace is a distillation of boost's thread.hpp, thread_data.hpp.
*/
namespace detail {
struct thread_data_base {
    thread_data_base() {}

    virtual ~thread_data_base() {}

    virtual void run() = 0;
};

typedef ::jge::shared_ptr<detail::thread_data_base> thread_data_ptr;

template <typename F>
class thread_data : public detail::thread_data_base {
public:
    thread_data(F f_) : f(f_) {}

    thread_data(thread_data&)    = delete;
    void operator=(thread_data&) = delete;

    void run() { f(); }

private:
    F f;
};

}  // namespace detail

/**
 ** A simplistic implementation of boost::thread, using pspsdk calls for the thread invocation.
 **
 ** The intent of its usage is this form only:
 ** mWorkerThread = boost::thread(ThreadProc, this);
 ** where ThreadProc is a static member function of the 'this' class,eg:
 ** static void FOO::ThreadProc(void* inParam)
 ** {
 **		FOO* instance = reinterpret_cast<FOO*>(inParam);
 **		// now you have class instance data available...
 ** }
 **
 ** Any other variant of a thread proc with more than one param is unimplemented.
 */
class thread {
    /*
    ** Helper class for sceKernelStartThread, which passes args by value, not by reference
    ** We use this struct to wrap any pointers that we want to pass to the worker thread.
    */
    struct CallbackData {
        CallbackData(detail::thread_data_ptr thread_info) : m_thread_info(thread_info) {}

        detail::thread_data_ptr m_thread_info;
    };

public:
    thread() {}

    template <class F, class A1>
    thread(F f, A1 a1) : m_thread_info(make_thread_info(std::bind(f, a1))) {
        LOG("Calling bind on threadproc func");
        CallbackData callbackData(m_thread_info);

        LOG("Creating SCE Thread");
        m_thread_id =
            ::sceKernelCreateThread(typeid(a1).name(), thread::ThreadProc, 0x15, 0x40000, PSP_THREAD_ATTR_USER, NULL);
        if (m_thread_id > 0) {
            ::sceKernelStartThread(m_thread_id, sizeof(CallbackData), &callbackData);
        }
    }

    ~thread() {}

    void join() { ::sceKernelTerminateDeleteThread(m_thread_id); }

private:
    static int ThreadProc(SceSize args, void* inParam) {
        LOG("Entering thread::ThreadProc");
        CallbackData* callbackData = reinterpret_cast<CallbackData*>(inParam);
        if (callbackData) {
            callbackData->m_thread_info->run();
        }

        return 0;
    }

    template <typename F>
    static inline detail::thread_data_ptr make_thread_info(F f) {
        return detail::thread_data_ptr(new detail::thread_data<F>(f));
    }

    detail::thread_data_ptr m_thread_info;
    SceUID m_thread_id;
};

}  // namespace jge

#endif  // JGE_PLATFORM_PSP_THREAD_HPP
