#ifndef CPR_THREADPOOL_BACKEND_H
#define CPR_THREADPOOL_BACKEND_H

#include "cpr/threadpool.h"

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>

namespace cpr {

/**
 * Internal scheduling contract shared by the builtin and Boost.Asio backends.
 * The facade uses mutex to read configuration and counters. Derived backends use
 * the same mutex for dispatch, queue access and worker accounting, so claiming a
 * task and deciding whether to retire a worker cannot race with submission.
 * Lifecycle transitions use lifecycleMutex before mutex; worker execution never
 * holds lifecycleMutex. Concrete implementations and Boost headers remain outside this header.
 **/
class ThreadPoolBackend {
  public:
    /**
     * Records worker execution on the calling thread for the lifetime of this scope.
     * A linked stack preserves outer pools when a handler drives nested context
     * dispatch, so lifecycle calls cannot wait on any suspended task on this thread.
     **/
    class WorkerScope {
      public:
        /** Registers backend as an active pool on this thread, preserving the previous scope. */
        explicit WorkerScope(const ThreadPoolBackend& backend) : backend(&backend) {
            currentWorkerScope = this;
        }
        /** Restores the outer worker scope when execution finishes or unwinds. */
        ~WorkerScope() {
            currentWorkerScope = previous;
        }
        /** Copying is disabled because this object is linked into the thread's worker stack. */
        WorkerScope(const WorkerScope&) = delete;
        /** Assignment is disabled because this object's stack identity must remain stable. */
        WorkerScope& operator=(const WorkerScope&) = delete;

      private:
        friend class ThreadPoolBackend;
        /** Borrowed backend kept alive by its worker or active handler throughout this scope. */
        const ThreadPoolBackend* backend;
        /** Outer scope, or nullptr at the bottom of the calling thread's worker stack. */
        const WorkerScope* previous{currentWorkerScope};
    };

    /**
     * Initializes a stopped backend and validates its worker limits without creating workers.
     * @param minimum Minimum builtin worker count, retained as configuration for Boost.
     * @param maximum Nonzero maximum worker count, no less than minimum.
     * @throws std::invalid_argument If the limits are invalid.
     **/
    ThreadPoolBackend(size_t minimum, size_t maximum) : minimum(minimum), maximum(maximum) {
        validate(minimum, maximum);
    }
    /** Releases shared state; derived destructors must first stop and join their workers. */
    virtual ~ThreadPoolBackend() = default;
    /** @copydoc ThreadPool::Start() */
    virtual void Start() = 0;
    /** @copydoc ThreadPool::Stop() */
    virtual void Stop() = 0;
    /**
     * Takes ownership of a task, dispatching it when running or retaining it while stopped.
     * @param task Copyable packaged-task wrapper; user exceptions are captured in its future.
     * @note Queue allocation and executor or worker creation failures propagate to the caller.
     **/
    virtual void Enqueue(std::function<void()> task) = 0;
    /**
     * Validates and updates selected limits under the same lock as the update.
     * This keeps individual setters atomic with respect to concurrent limit changes.
     * @param minimum Pointer to the new minimum, or nullptr to retain the current value.
     * @param maximum Pointer to the new maximum, or nullptr to retain the current value.
     *        Pointers are read during the call and are not retained by the backend.
     * @throws std::invalid_argument If the resulting limits are invalid.
     * @throws std::logic_error If called from this backend's worker.
     **/
    virtual void Configure(const size_t* minimum, const size_t* maximum) = 0;

    /** @copydoc ThreadPool::Wait() */
    void Wait() const {
        checkWorker();
        std::unique_lock lock(mutex);
        changed.wait(lock, [this] { return state == ThreadPool::State::STOP ? !stopping : tasks.empty() && active == 0; });
    }

    /**
     * Checks limits without mutating configuration or creating workers.
     * @param minimum Requested minimum worker count.
     * @param maximum Requested nonzero maximum, no less than minimum.
     * @throws std::invalid_argument If the limits are invalid.
     **/
    static void validate(size_t minimum, size_t maximum) {
        if (maximum == 0 || minimum > maximum) {
            throw std::invalid_argument("ThreadPool requires 0 <= minimum <= maximum and maximum > 0");
        }
    }
    /**
     * Rejects lifecycle and configuration calls that could wait on their own worker.
     * @throws std::logic_error If the calling thread is executing this backend's task or worker loop.
     **/
    void checkWorker() const {
        for (const auto* scope = currentWorkerScope; scope != nullptr; scope = scope->previous) {
            if (scope->backend == this) {
                throw std::logic_error("ThreadPool lifecycle operations cannot be called from its workers");
            }
        }
    }

    /** Protects state, stopping, worker limits, counters and the task queue. */
    mutable std::mutex mutex;
    /** Wakes completion waiters and builtin workers after queue, state or counter changes. */
    mutable std::condition_variable changed;
    /**
     * Serializes Start()/Stop() and Boost reconfiguration while joins release mutex.
     * Acquire before mutex when both are needed, keeping Submit() usable during joins.
     **/
    std::mutex lifecycleMutex;
    /** Dispatch state, protected by mutex; STOP may include active workers still finishing. */
    ThreadPool::State state{ThreadPool::State::STOP};
    /** Whether shutdown or Boost replacement is joining workers; protected by mutex. */
    bool stopping{false};
    /** Configured minimum builtin worker count, retained for Boost; protected by mutex. */
    size_t minimum;
    /** Worker ceiling, fixed owned Boost worker count, or borrowed-context task limit; protected by mutex. */
    size_t maximum;
    /** Live owned workers, or active cpr workers on a borrowed context; protected by mutex. */
    size_t current{0};
    /** Tasks claimed by workers but not yet released after invocation; protected by mutex. */
    size_t active{0};
    /** Tasks not yet claimed, including submissions while stopped; protected by mutex. */
    std::queue<std::function<void()>> tasks;
    /** Top of the calling thread's worker stack, or nullptr outside worker execution. */
    inline static thread_local const WorkerScope* currentWorkerScope{nullptr};
};

/**
 * Creates the backend selected by CPR_THREAD_POOL without starting its workers.
 * Exactly one backend implementation supplies this factory in each cpr build.
 * @param minimum Minimum builtin worker count, retained as configuration for Boost.
 * @param maximum Nonzero maximum worker count, no less than minimum.
 * @return Shared backend state, initially stopped; the facade owns it for the pool lifetime.
 * @throws std::invalid_argument If the limits are invalid.
 **/
std::shared_ptr<ThreadPoolBackend> MakeThreadPoolBackend(size_t minimum, size_t maximum);

/**
 * Creates a stopped backend borrowing a caller-owned Boost.Asio context.
 * Posted handlers borrow the backend through weak pointers so they can safely be
 * dispatched after the facade is destroyed. The facade stops work before releasing ownership.
 * @param context Context that outlives the facade, driven by the caller.
 * @param minimum Retained minimum worker setting.
 * @param maximum Nonzero maximum number of concurrent cpr tasks, no less than minimum.
 * @return Shared backend state, initially stopped, without owning context or creating workers.
 * @throws std::invalid_argument If the limits are invalid.
 * @throws std::logic_error If cpr was built with the builtin backend.
 **/
std::shared_ptr<ThreadPoolBackend> MakeThreadPoolBackend(boost::asio::io_context& context, size_t minimum, size_t maximum);

} // namespace cpr
#endif
