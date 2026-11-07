#ifndef CPR_THREADPOOL_H
#define CPR_THREADPOOL_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#endif

// Forward declaration keeps the optional Boost dependency out of cpr's public headers.
namespace boost::asio {
class io_context;
} // namespace boost::asio

namespace cpr {
/**
 * Thread pool used by asynchronous requests, with its backend selected at build time.
 * CPR_THREAD_POOL=BUILTIN (the default) uses standard-library workers and scales
 * between the configured minimum and maximum. CPR_THREAD_POOL=BOOST_ASIO uses
 * boost::asio::thread_pool with a fixed worker count equal to the maximum.
 * Changing the Boost maximum waits for active tasks and replaces the executor,
 * preserving queued tasks. The context constructor instead borrows a caller-owned
 * event loop and limits concurrent cpr tasks without owning any context threads.
 *
 * Operations synchronize access to the pool, but the caller must ensure its
 * lifetime covers every concurrent call. Start(), Stop(), Wait() and limit setters
 * must be called outside this pool's workers; otherwise they throw std::logic_error.
 * Destruction must also happen outside its workers. Tasks must not block on other
 * tasks submitted to the same saturated pool, since no worker may be available.
 *
 * @code
 * cpr::ThreadPool pool(0, 4);
 * auto result = pool.Submit([] { return 42; });
 * pool.Wait();
 * pool.Stop();
 * pool.Start();
 * @endcode
 **/
EXPORT_CPR class ThreadPool {
  public:
    /** Whether workers may take queued tasks; STOP can still have active workers finishing tasks. */
    enum class State : uint8_t {
        STOP,   ///< Queued tasks are not dispatched, including during Boost executor replacement.
        RUNNING ///< Workers may take queued tasks.
    };

    /** Default minimum: zero, so the builtin backend creates workers only when needed. */
    static constexpr size_t DEFAULT_MIN_THREAD_COUNT = 0;
    /**
     * Default maximum, initialized to hardware concurrency or one when it is unknown.
     * Set this before constructing pools, without concurrent access to this variable.
     **/
    static size_t DEFAULT_MAX_THREAD_COUNT;

    /**
     * Creates and starts a pool with the requested limits.
     * BUILTIN starts minThreadCount workers; BOOST_ASIO starts maxThreadCount workers.
     *
     * @param minThreadCount Minimum builtin worker count, including idle workers.
     *        Boost retains this setting without reducing its fixed worker count.
     * @param maxThreadCount Maximum worker count; must be greater than zero.
     * @throws std::invalid_argument If the minimum exceeds the maximum or the maximum is zero.
     * @note Allocation and worker creation failures propagate to the caller.
     **/
    explicit ThreadPool(size_t minThreadCount = DEFAULT_MIN_THREAD_COUNT, size_t maxThreadCount = DEFAULT_MAX_THREAD_COUNT);
    /**
     * Creates a pool dispatching cpr tasks through a caller-owned Boost.Asio context.
     * Requires CPR_THREAD_POOL=BOOST_ASIO and matching Boost headers and Asio configuration
     * between cpr and the caller. cpr creates no workers and never calls
     * run(), restart() or stop() on context; the caller drives its event loop.
     * The minimum is retained as configuration; the maximum limits concurrent cpr
     * tasks without limiting unrelated handlers. Current worker counts reflect
     * active cpr tasks only, and the reported idle worker count is always zero.
     * @param context Context that outlives this pool; keep its event loop running
     *        when waiting for cpr tasks, and restart it yourself if it has stopped.
     * @param minThreadCount Retained minimum setting, no greater than maxThreadCount.
     * @param maxThreadCount Nonzero maximum number of concurrent cpr tasks.
     * @throws std::invalid_argument If the worker limits are invalid.
     * @throws std::logic_error If cpr was built with the builtin backend.
     **/
    explicit ThreadPool(boost::asio::io_context& context, size_t minThreadCount = DEFAULT_MIN_THREAD_COUNT, size_t maxThreadCount = DEFAULT_MAX_THREAD_COUNT);
    /** Copying is disabled because worker and task ownership cannot be shared between pools. */
    ThreadPool(const ThreadPool& other) = delete;
    /** Moving is disabled because workers retain references to their owning backend. */
    ThreadPool(ThreadPool&& old) = delete;
    /**
     * Stops the pool, cancels queued tasks and joins active workers.
     * @pre Destruction happens outside this pool's workers, with no concurrent calls using the pool.
     **/
    virtual ~ThreadPool();

    /** Copy assignment is disabled; the pool exclusively owns its backend. */
    ThreadPool& operator=(const ThreadPool& other) = delete;
    /** Move assignment is disabled; workers retain references to their owning backend. */
    ThreadPool& operator=(ThreadPool&& old) = delete;

    /**
     * Returns a synchronized snapshot of whether workers may take queued tasks.
     * STOP does not guarantee workers have finished; Stop() and Wait() wait for shutdown.
     * Boost also reports STOP while replacing its executor after a maximum change.
     * @return RUNNING when dispatch is enabled, otherwise STOP.
     **/
    [[nodiscard]] State GetState() const;
    /**
     * Returns the configured maximum worker count.
     * After reducing the builtin maximum, existing workers retire as their tasks finish.
     * For a caller-owned context this limits simultaneous cpr task execution.
     * @return The maximum used for workers or concurrent tasks on a caller-owned context.
     **/
    [[nodiscard]] size_t GetMaxThreadCount() const;
    /**
     * Returns a synchronized snapshot of live workers, including active and idle workers.
     * This may temporarily exceed a reduced maximum while previous tasks finish or builtin workers retire.
     * With a caller-owned context this reports active cpr tasks; its total thread count is not known.
     * @return Current worker count; zero after Stop() finishes.
     **/
    [[nodiscard]] size_t GetCurThreadCount() const;
    /**
     * Returns a synchronized snapshot of workers not currently executing a task.
     * Queued work may exist while such workers are about to take a task.
     * Caller-owned contexts report zero because their idle threads are not managed by cpr.
     * @return Current worker count minus active task count.
     **/
    [[nodiscard]] size_t GetIdleThreadCount() const;
    /**
     * Returns the configured minimum worker count.
     * The owned Boost executor always uses the maximum number of workers while running.
     * Caller-owned contexts retain this setting without creating workers.
     * @return The minimum number of workers retained by the builtin backend.
     **/
    [[nodiscard]] size_t GetMinThreadCount() const;

    /**
     * Sets the minimum and grows a running builtin pool when necessary.
     * Reducing the minimum allows excess builtin workers to retire after their idle timeout.
     * This setting does not change the Boost worker count or create workers for a caller-owned context.
     * @param minThreadCount New minimum, no greater than the configured maximum.
     * @throws std::invalid_argument If the minimum exceeds the maximum.
     * @throws std::logic_error If called from this pool's worker.
     **/
    void SetMinThreadCount(size_t minThreadCount);
    /**
     * Sets the maximum, preserving queued tasks.
     * Excess builtin workers retire as their tasks finish. Changing a running Boost
     * pool's maximum blocks until active tasks finish and replaces its executor.
     * Caller-owned contexts retain their executor and apply the new concurrency limit
     * to subsequent task claims; active tasks finish normally.
     * @param maxThreadCount New nonzero maximum, no less than the configured minimum.
     * @throws std::invalid_argument If zero or less than the configured minimum.
     * @throws std::logic_error If called from this pool's worker.
     **/
    void SetMaxThreadCount(size_t maxThreadCount);
    /**
     * Validates and updates both limits together, avoiding invalid intermediate limits.
     * Backend resizing follows SetMinThreadCount() and SetMaxThreadCount(). Invalid
     * limits leave configuration unchanged; allocation and worker creation failures propagate.
     * @param minThreadCount New minimum worker count.
     * @param maxThreadCount New nonzero maximum, no less than minThreadCount.
     * @throws std::invalid_argument If the minimum exceeds the maximum or the maximum is zero.
     * @throws std::logic_error If called from this pool's worker.
     **/
    void SetThreadCounts(size_t minThreadCount, size_t maxThreadCount);

    /**
     * Starts workers and processes tasks submitted while stopped.
     * BUILTIN starts enough workers for the minimum and queued tasks, up to the maximum.
     * The owned BOOST_ASIO executor starts the configured maximum number of workers.
     * Caller-owned contexts receive posted work without creating or starting event-loop threads.
     * Does nothing when already RUNNING. Concurrent Start()/Stop() calls are serialized.
     * @throws std::logic_error If called from this pool's worker.
     * @note Allocation and worker creation failures propagate to the caller.
     **/
    void Start();
    /**
     * Disables dispatch, cancels tasks queued at that moment, and joins active workers.
     * Canceled futures receive std::future_errc::broken_promise. Tasks submitted after
     * dispatch stops are retained for the next Start(); another Stop() cancels them.
     * A caller-owned context is left running; only cpr tasks are canceled or awaited.
     * Concurrent Start()/Stop() calls are serialized. Calling Stop() on an already
     * stopped pool is supported and clears any tasks queued since its previous stop.
     * @throws std::logic_error If called from this pool's worker.
     **/
    void Stop();
    /**
     * Blocks until a running pool has no queued or active tasks, without stopping it.
     * For a stopped pool, waits for shutdown or executor replacement to finish;
     * work queued for a later Start() does not prevent return while stopped.
     * Concurrent submissions may add work after Wait() returns.
     * A caller-owned context must be driven by another thread while this call blocks;
     * waiting on its only event-loop thread prevents queued tasks from running.
     * @throws std::logic_error If called from this pool's worker.
     **/
    void Wait() const;

    /**
     * Submits a callable and returns a future for its result, including void or reference results.
     * The callable and arguments are copied or moved into owned storage; move-only
     * values and member pointers are supported. Use std::ref() for borrowed references
     * and keep the referenced objects alive until execution finishes or the task is canceled.
     * Stored values are moved into invocation. Callable exceptions are delivered by future.get().
     * Reference results do not extend the lifetime of the objects they refer to.
     *
     * BUILTIN may create workers when queued work exceeds available workers, up to the
     * maximum. BOOST_ASIO posts work to its fixed-size executor. While stopped, work
     * remains queued until Start() or is canceled by Stop() or destruction.
     * @tparam Fn Callable type, deduced from fn.
     * @tparam Args Argument types, deduced from args.
     * @param fn Callable to execute on a worker.
     * @param args Arguments forwarded into the task's owned storage.
     * @return Future for the invocation result; cancellation produces broken_promise.
     * @note Submission, allocation and worker creation failures propagate directly to the caller.
     **/
    template <class Fn, class... Args>
    auto Submit(Fn&& fn, Args&&... args) {
        using RetType = std::invoke_result_t<std::decay_t<Fn>, std::decay_t<Args>...>;
        auto task = std::make_shared<std::packaged_task<RetType()>>([fn = std::forward<Fn>(fn), arguments = std::make_tuple(std::forward<Args>(args)...)]() mutable -> RetType { return std::apply(std::move(fn), std::move(arguments)); });
        auto future = task->get_future();
        enqueue([task] { (*task)(); });
        return future;
    }

  private:
    /** Opaque implementation hiding backend types and dependencies from public headers. */
    class Impl;
    /** Exclusively owned implementation; its lifetime covers all worker activity. */
    std::unique_ptr<Impl> impl;
    /**
     * Transfers a packaged-task wrapper to the selected backend's queue.
     * @param task Copyable wrapper owning the packaged task that fulfills Submit()'s future.
     **/
    void enqueue(std::function<void()> task);
};
} // namespace cpr
#endif
