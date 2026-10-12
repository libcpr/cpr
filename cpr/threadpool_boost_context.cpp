#include "cpr/threadpool.h"
#include "cpr/threadpool_backend.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>

namespace cpr {
namespace {
/**
 * Boost backend borrowing an application's event loop without owning its threads.
 * Posted handlers keep weak references to this state, so canceled handlers are
 * harmless after the facade is destroyed. Active handlers temporarily retain
 * shared ownership while Stop() waits for their task and capture cleanup.
 **/
// NOLINTNEXTLINE(fuchsia-multiple-inheritance, misc-multiple-inheritance) enable_shared_from_this supplies handler lifetime tracking.
class BoostContextThreadPool final : public ThreadPoolBackend, public std::enable_shared_from_this<BoostContextThreadPool> {
  public:
    /**
     * Initializes a stopped scheduler without modifying or driving the context.
     * @param context Caller-owned context that outlives the facade.
     * @param minimum Retained minimum setting; no context threads are created.
     * @param maximum Nonzero concurrency limit for cpr tasks, no less than minimum.
     * @throws std::invalid_argument If the limits are invalid.
     **/
    BoostContextThreadPool(boost::asio::io_context& context, size_t minimum, size_t maximum) : ThreadPoolBackend(minimum, maximum), context(context) {}
    /** Cancels cpr work and waits for its active tasks, without stopping the caller's event loop. */
    ~BoostContextThreadPool() override {
        try {
            Stop();
        } catch (...) {
            // Destruction cannot propagate shutdown errors or continue with live workers.
            std::terminate();
        }
    }

    /** @copydoc ThreadPoolBackend::Start() */
    void Start() override {
        checkWorker();
        const std::scoped_lock lifecycleLock(lifecycleMutex);
        const std::scoped_lock lock(mutex);
        if (state == ThreadPool::State::RUNNING) {
            return;
        }
        state = ThreadPool::State::RUNNING;
        postPending();
        changed.notify_all();
    }

    /** @copydoc ThreadPoolBackend::Stop() */
    void Stop() override {
        checkWorker();
        const std::scoped_lock lifecycleLock(lifecycleMutex);
        std::queue<std::function<void()>> canceled;
        {
            const std::scoped_lock lock(mutex);
            state = ThreadPool::State::STOP;
            stopping = true;
            tasks.swap(canceled);
        }
        canceled = {};
        std::unique_lock lock(mutex);
        changed.wait(lock, [this] { return active == 0; });
        stopping = false;
        changed.notify_all();
    }

    /**
     * @copydoc ThreadPoolBackend::Configure()
     * Updates the cpr concurrency limit without modifying the context or waiting
     * for active tasks. Extra active tasks finish normally after a reduction.
     **/
    void Configure(const size_t* newMinimum, const size_t* newMaximum) override {
        checkWorker();
        const std::scoped_lock lock(mutex);
        const size_t min = newMinimum ? *newMinimum : minimum;
        const size_t max = newMaximum ? *newMaximum : maximum;
        validate(min, max);
        minimum = min;
        maximum = max;
        if (state == ThreadPool::State::RUNNING) {
            postPending();
        }
        changed.notify_all();
    }

    /** @copydoc ThreadPoolBackend::Enqueue() */
    void Enqueue(std::function<void()> task) override {
        const std::scoped_lock lock(mutex);
        if (state == ThreadPool::State::RUNNING) {
            postOne();
        }
        tasks.emplace(std::move(task));
    }

  private:
    /** Borrowed event loop; cpr never calls its run(), restart(), stop() or destructor. */
    boost::asio::io_context& context;

    /**
     * Posts a handler that claims at most one cpr task and is safe after pool destruction.
     * @pre mutex is held and this backend has been created through shared ownership.
     * @note Posting failures propagate to the caller; post() does not invoke handlers inline.
     **/
    // Asio post() never invokes this handler inline, so reposting does not recurse.
    // NOLINTBEGIN(misc-no-recursion)
    void postOne() {
        boost::asio::post(context, [weak = weak_from_this()] {
            if (auto backend = weak.lock()) {
                backend->executeOne();
            }
        });
    }

    /** Posts handlers for pending tasks; mutex is held and dispatch is RUNNING. */
    void postPending() {
        for (size_t i = 0; i < tasks.size(); ++i) {
            postOne();
        }
    }

    /**
     * Claims a task if dispatch and the concurrency limit permit it, then invokes
     * it outside mutex. Reports active cpr tasks as current workers, since the context
     * thread count is unknown. Keeps a worker scope for nested event-loop safety.
     * When saturated handlers return, task completion posts another handler for
     * remaining work, allowing unrelated context handlers to run between cpr tasks.
     **/
    void executeOne() {
        std::function<void()> task;
        {
            const std::scoped_lock lock(mutex);
            if (state == ThreadPool::State::STOP || tasks.empty() || active >= maximum) {
                return;
            }
            task = std::move(tasks.front());
            tasks.pop();
            ++active;
            ++current;
        }
        const WorkerScope workerScope(*this);
        task();
        task = {};
        const std::scoped_lock lock(mutex);
        --active;
        --current;
        changed.notify_all();
        // Keep posting inside the lock so Stop() cannot return while a callback
        // still accesses the borrowed context during its completion bookkeeping.
        if (state == ThreadPool::State::RUNNING && !tasks.empty() && active < maximum) {
            postOne();
        }
    }
    // NOLINTEND(misc-no-recursion)
};
} // namespace

std::shared_ptr<ThreadPoolBackend> MakeThreadPoolBackend(boost::asio::io_context& context, size_t minimum, size_t maximum) {
    return std::make_shared<BoostContextThreadPool>(context, minimum, maximum);
}
} // namespace cpr
