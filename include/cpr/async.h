#ifndef CPR_ASYNC_H
#define CPR_ASYNC_H

#include "async_wrapper.h"
#include "singleton.h"
#include "threadpool.h"

namespace cpr {

class GlobalThreadPool : public ThreadPool {
    CPR_SINGLETON_DECL(GlobalThreadPool)
  protected:
    GlobalThreadPool() = default;

  public:
    ~GlobalThreadPool() override = default;
};

/**
 * Return a wrapper for a future, calling future.get() will wait until the task is done and return RetType.
 * async(fn, args...)
 * async(std::bind(&Class::mem_fn, &obj))
 * async(std::mem_fn(&Class::mem_fn, &obj))
 **/
template <class Fn, class... Args>
auto async(Fn&& fn, Args&&... args) {
    return AsyncWrapper{GlobalThreadPool::GetInstance()->Submit(std::forward<Fn>(fn), std::forward<Args>(args)...)};
}

class async {
  public:
    static void startup(size_t minThreads = ThreadPool::DEFAULT_MIN_THREAD_COUNT, size_t maxThreads = ThreadPool::DEFAULT_MAX_THREAD_COUNT) {
        GlobalThreadPool* gtp = GlobalThreadPool::GetInstance();
        gtp->SetMinThreadCount(minThreads);
        gtp->SetMaxThreadCount(maxThreads);
    }

    static void cleanup() {
        GlobalThreadPool::ExitInstance();
    }
};

} // namespace cpr

#endif
