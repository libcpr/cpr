#ifndef CPR_ASYNC_H
#define CPR_ASYNC_H

#include "singleton.h"
#include "threadpool.h"

namespace cpr {

class GlobalThreadPool : public ThreadPool {
    SINGLETON_DECL(GlobalThreadPool)
protected:
    GlobalThreadPool() : ThreadPool() {}
    ~GlobalThreadPool() {}
};

/*
 * return a future, calling future.get() will wait task done and return RetType.
 * async(fn, args...)
 * async(std::bind(&Class::mem_fn, &obj))
 * async(std::mem_fn(&Class::mem_fn, &obj))
 *
 */
template<class Fn, class... Args>
auto async(Fn&& fn, Args&&... args) -> std::future<decltype(fn(args...))> {
    return GlobalThreadPool::instance()->commit(std::forward<Fn>(fn), std::forward<Args>(args)...);
}

class async {
public:
    static void startup(int min_threads = DEFAULT_THREAD_POOL_MIN_THREAD_NUM,
                 int max_threads = DEFAULT_THREAD_POOL_MAX_THREAD_NUM,
                 int max_idle_ms = DEFAULT_THREAD_POOL_MAX_IDLE_TIME) {
        GlobalThreadPool* gtp = GlobalThreadPool::instance();
        if (gtp->isStarted()) return;
        gtp->setMinThreadNum(min_threads);
        gtp->setMaxThreadNum(max_threads);
        gtp->setMaxIdleTime(max_idle_ms);
        gtp->start();
    }

    static void cleanup() {
        GlobalThreadPool::exitInstance();
    }
};

} // namespace cpr

#endif
