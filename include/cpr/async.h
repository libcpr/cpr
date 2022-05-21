#ifndef CPR_ASYNC_H
#define CPR_ASYNC_H

#include "singleton.h"
#include "threadpool.h"

namespace cpr {

class GlobalThreadPool : public ThreadPool {
    CPR_SINGLETON_DECL(GlobalThreadPool)
  protected:
    GlobalThreadPool() : ThreadPool() {}
    ~GlobalThreadPool() {}
};

/**
 * Return a future, calling future.get() will wait task done and return RetType.
 * async(fn, args...)
 * async(std::bind(&Class::mem_fn, &obj))
 * async(std::mem_fn(&Class::mem_fn, &obj))
 **/
template<class Fn, class... Args>
auto async(Fn&& fn, Args&&... args) -> std::future<decltype(fn(args...))> {
    return GlobalThreadPool::GetInstance()->Submit(std::forward<Fn>(fn), std::forward<Args>(args)...);
}

class async {
  public:
    static void startup(size_t min_threads = CPR_DEFAULT_THREAD_POOL_MIN_THREAD_NUM,
                 size_t max_threads = CPR_DEFAULT_THREAD_POOL_MAX_THREAD_NUM,
                 std::chrono::milliseconds max_idle_ms = CPR_DEFAULT_THREAD_POOL_MAX_IDLE_TIME) {
        GlobalThreadPool* gtp = GlobalThreadPool::GetInstance();
        if (gtp->IsStarted()) return;
        gtp->SetMinThreadNum(min_threads);
        gtp->SetMaxThreadNum(max_threads);
        gtp->SetMaxIdleTime(max_idle_ms);
        gtp->Start();
    }

    static void cleanup() {
        GlobalThreadPool::ExitInstance();
    }
};

} // namespace cpr

#endif
