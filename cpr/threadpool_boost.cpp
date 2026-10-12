#include "cpr/threadpool.h"
#include "cpr/threadpool_backend.h"

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
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
 * Boost.Asio backend with a fixed-size executor containing maximum workers.
 * Task ownership stays in the shared queue, while posted handlers claim one task
 * each. This lets Stop() cancel pending futures immediately and lets Configure()
 * replace the executor without discarding queued work.
 **/
class BoostThreadPool final : public ThreadPoolBackend {
  public:
    /** @copydoc ThreadPoolBackend::ThreadPoolBackend() */
    BoostThreadPool(size_t minimum, size_t maximum) : ThreadPoolBackend(minimum, maximum) {}
    /** Stops and joins the executor before releasing the queue and backend state. */
    ~BoostThreadPool() override {
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
        launch();
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
            if (pool) {
                pool->stop();
            }
        }
        canceled = {};
        join();
        {
            const std::scoped_lock lock(mutex);
            stopping = false;
        }
        changed.notify_all();
    }

    /** @copydoc ThreadPoolBackend::Configure() */
    void Configure(const size_t* newMinimum, const size_t* newMaximum) override {
        checkWorker();
        const std::scoped_lock lifecycleLock(lifecycleMutex);
        std::unique_lock lock(mutex);
        const size_t min = newMinimum ? *newMinimum : minimum;
        const size_t max = newMaximum ? *newMaximum : maximum;
        validate(min, max);
        if (state == ThreadPool::State::RUNNING && max != maximum) {
            // Asio pools are fixed-size. Finish active tasks and preserve queued tasks
            // while replacing the executor. Submit remains usable during this transition.
            state = ThreadPool::State::STOP;
            stopping = true;
            pool->stop();
            lock.unlock();
            join();
            lock.lock();
            minimum = min;
            maximum = max;
            stopping = false;
            changed.notify_all(); // Also wake Wait() if allocating the replacement pool throws.
            launch();
        } else {
            minimum = min;
            maximum = max;
        }
        changed.notify_all();
    }

    /** @copydoc ThreadPoolBackend::Enqueue() */
    void Enqueue(std::function<void()> task) override {
        const std::scoped_lock lock(mutex);
        if (state == ThreadPool::State::RUNNING) {
            boost::asio::post(*pool, [this] { executeOne(); });
        }
        tasks.emplace(std::move(task));
    }

  private:
    /**
     * Owned Asio executor, absent before Start() and after joining stopped workers.
     * Posting and stopping hold mutex; creation and joining hold lifecycleMutex.
     * Joining happens without mutex while dispatch is disabled, allowing active
     * tasks to finish or submit work for the next executor.
     **/
    std::unique_ptr<boost::asio::thread_pool> pool;

    /**
     * Creates maximum workers, enables dispatch and posts handlers for all pending tasks.
     * @pre lifecycleMutex and mutex are held; any previous executor has been joined.
     * @note Allocation, executor creation and posting failures propagate to the caller.
     **/
    void launch() {
        pool = std::make_unique<boost::asio::thread_pool>(maximum);
        current = maximum;
        state = ThreadPool::State::RUNNING;
        for (size_t i = 0; i < tasks.size(); ++i) {
            boost::asio::post(*pool, [this] { executeOne(); });
        }
        changed.notify_all();
    }

    /**
     * Waits for stopped executor workers, releases the executor and resets current to zero.
     * Tasks in the shared queue are retained for the caller to cancel or redispatch.
     * @pre lifecycleMutex is held, dispatch is STOP and mutex is not held.
     **/
    void join() {
        if (pool) {
            pool->join();
            pool.reset();
        }
        const std::scoped_lock lock(mutex);
        current = 0;
    }

    /**
     * Claims one queued task when dispatch is enabled, otherwise returns without claiming it.
     * Invokes the task outside mutex and marks this thread as a backend worker to
     * reject self-waiting lifecycle calls. Releases captures before decrementing
     * active and notifying Wait() that execution and cleanup have finished.
     **/
    void executeOne() {
        std::function<void()> task;
        {
            const std::scoped_lock lock(mutex);
            if (state == ThreadPool::State::STOP || tasks.empty()) {
                return;
            }
            task = std::move(tasks.front());
            tasks.pop();
            ++active;
        }
        const WorkerScope workerScope(*this);
        task();
        task = {};
        {
            const std::scoped_lock lock(mutex);
            --active;
        }
        changed.notify_all();
    }
};
} // namespace

std::shared_ptr<ThreadPoolBackend> MakeThreadPoolBackend(size_t minimum, size_t maximum) {
    return std::make_shared<BoostThreadPool>(minimum, maximum);
}
} // namespace cpr
