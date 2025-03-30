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
template <bool isCancellable = false, class Fn, class... Args>
auto async(Fn&& fn, Args&&... args) {
  std::future future = GlobalThreadPool::GetInstance()->Submit(std::forward<Fn>(fn), std::forward<Args>(args)...);
  using async_wrapper_t = AsyncWrapper<decltype(future.get()), isCancellable>;
  if constexpr (isCancellable) {
    return async_wrapper_t{std::move(future), std::make_shared<std::atomic_bool>(false)};
  } else {
    return async_wrapper_t{std::move(future)};
  }
}

class async {
  public:
    static void startup(size_t min_threads = CPR_DEFAULT_THREAD_POOL_MIN_THREAD_NUM, size_t max_threads = CPR_DEFAULT_THREAD_POOL_MAX_THREAD_NUM, std::chrono::milliseconds max_idle_ms = CPR_DEFAULT_THREAD_POOL_MAX_IDLE_TIME) {
        GlobalThreadPool* gtp = GlobalThreadPool::GetInstance();
        if (gtp->IsStarted()) {
            return;
        }
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
