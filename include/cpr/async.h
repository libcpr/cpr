#ifndef CPR_ASYNC_H
#define CPR_ASYNC_H

#include "cpr/export.h"

#include "async_wrapper.h"
#include "singleton.h"
#include "threadpool.h"

namespace cpr {

/**
 * Lazily created process-wide pool used by cpr::async() and asynchronous requests.
 * GetInstance() creates the selected backend with ThreadPool's default limits.
 * Its context overload instead selects a caller-owned event loop on first use.
 * ExitInstance() destroys it once; the singleton cannot be recreated after cleanup.
 * Callers must coordinate shutdown with every use of this pool.
 **/
EXPORT_CPR class GlobalThreadPool : public ThreadPool {
    CPR_SINGLETON_DECL(GlobalThreadPool)
  protected:
    /** Constructs the singleton with the selected backend and default worker limits. */
    GlobalThreadPool() = default;
    /** Constructs the singleton borrowing the supplied context without creating workers. */
    explicit GlobalThreadPool(boost::asio::io_context& context) : ThreadPool(context), context(&context) {}

  public:
    /**
     * Creates the singleton using a caller-owned context on its first initialization.
     * Later calls with the same context return the existing instance. A different
     * context or prior initialization with the default backend is rejected.
     * @param context Context that outlives the global pool; the caller runs its event loop.
     * @return Global pool borrowing context.
     * @throws std::logic_error If the builtin backend was selected, initialization
     *         already chose another context/default executor, or cleanup() was called.
     * @pre Select the context before any call that creates the default global pool.
     **/
    static GlobalThreadPool* GetInstance(boost::asio::io_context& context);
    /** Cancels queued work and joins active workers through ThreadPool's destructor. */
    ~GlobalThreadPool() override = default;

  private:
    /** Borrowed context identity fixed at initialization, or nullptr for the default executor. */
    boost::asio::io_context* context{nullptr};
};

/**
 * Submits a callable to GlobalThreadPool and wraps its future.
 * Arguments follow ThreadPool::Submit() ownership rules, including std::ref()
 * for borrowed values. get() waits for the result and propagates task exceptions.
 * @tparam isCancellable Whether the returned wrapper exposes its cancellation interface.
 * @tparam Fn Callable type, deduced from fn.
 * @tparam Args Argument types, deduced from args.
 * @param fn Callable to execute on a global-pool worker.
 * @param args Arguments forwarded to ThreadPool::Submit().
 * @return AsyncWrapper holding the submitted task's future.
 * @pre The global pool has not been destroyed by cpr::async::cleanup().
 **/
EXPORT_CPR template <bool isCancellable = false, class Fn, class... Args>
auto async(Fn&& fn, Args&&... args) {
    std::future future = GlobalThreadPool::GetInstance()->Submit(std::forward<Fn>(fn), std::forward<Args>(args)...);
    using async_wrapper_t = AsyncWrapper<decltype(future.get()), isCancellable>;
    if constexpr (isCancellable) {
        return async_wrapper_t{std::move(future), std::make_shared<std::atomic_bool>(false)};
    } else {
        return async_wrapper_t{std::move(future)};
    }
}

/** Controls worker limits and final shutdown of the process-wide asynchronous request pool. */
EXPORT_CPR class async {
  public:
    /**
     * Lazily creates the global pool and sets both worker limits together.
     * A changed maximum on an owned Boost executor waits for active tasks and replaces
     * the executor, preserving queued tasks. Builtin pools and caller-owned contexts
     * apply the limits without joining active tasks; no borrowed event loop is changed.
     * @param minThreads Minimum builtin worker count, retained as configuration for Boost.
     * @param maxThreads Nonzero maximum worker count, no less than minThreads.
     * @throws std::invalid_argument If the limits are invalid.
     * @throws std::logic_error If called from a global-pool worker.
     * @pre cleanup() has not been called.
     **/
    static void startup(size_t minThreads = ThreadPool::DEFAULT_MIN_THREAD_COUNT, size_t maxThreads = ThreadPool::DEFAULT_MAX_THREAD_COUNT) {
        GlobalThreadPool* gtp = GlobalThreadPool::GetInstance();
        gtp->SetThreadCounts(minThreads, maxThreads);
    }

    /**
     * Selects a caller-owned Boost.Asio context on first use and configures cpr task concurrency.
     * This overload creates no workers, does not drive or restart the event loop,
     * and never stops unrelated work on the context. Requires CPR_THREAD_POOL=BOOST_ASIO.
     * @param context Context that outlives the global pool; the caller keeps it running when waiting for cpr tasks.
     * @param minThreads Retained minimum setting, no greater than maxThreads.
     * @param maxThreads Nonzero maximum number of concurrent cpr tasks on context.
     * @throws std::invalid_argument If the limits are invalid.
     * @throws std::logic_error If context selection is unsupported or conflicts with
     *         earlier initialization, cleanup() was called, or this is called from a cpr worker.
     * Invalid limits are rejected before selecting or creating the global pool.
     * @pre Call before creating the global pool through any other API.
     **/
    static void startup(boost::asio::io_context& context, size_t minThreads = ThreadPool::DEFAULT_MIN_THREAD_COUNT, size_t maxThreads = ThreadPool::DEFAULT_MAX_THREAD_COUNT);

    /**
     * Permanently destroys the global pool, canceling queued tasks and joining active workers.
     * Repeated calls after a completed cleanup have no effect; the singleton cannot restart.
     * @pre The global pool has been created. Call outside its workers, with no
     *      concurrent operations accessing it, and do not submit requests afterward.
     **/
    static void cleanup() {
        GlobalThreadPool::ExitInstance();
    }
};

} // namespace cpr

#endif
