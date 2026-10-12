#include "cpr/threadpool.h"
#include "cpr/threadpool_backend.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <exception>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>

namespace cpr {
namespace {
/** Idle interval before a builtin worker may retire when the current count exceeds the minimum. */
constexpr std::chrono::milliseconds THREAD_IDLE_TIMEOUT{250};

/**
 * Standard-library backend with workers created on demand and retired after idle periods.
 * Worker counts and task claims share the queue mutex, preventing the last worker
 * from retiring after submission has decided that an existing worker can take its task.
 * Finished thread handles are reclaimed when growing the pool or stopping it.
 **/
class BuiltinThreadPool final : public ThreadPoolBackend {
  public:
    /** @copydoc ThreadPoolBackend::ThreadPoolBackend() */
    BuiltinThreadPool(size_t minimum, size_t maximum) : ThreadPoolBackend(minimum, maximum) {}
    /** Stops dispatch, cancels pending tasks and joins workers before their storage is destroyed. */
    ~BuiltinThreadPool() override {
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
        grow(std::min(maximum, std::max(minimum, tasks.size())));
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
        // Destroy canceled packaged tasks before joining, so their futures become ready immediately.
        canceled = {};
        changed.notify_all();
        for (auto& worker : workers) {
            worker.thread.join();
        }
        workers.clear();
        {
            const std::scoped_lock lock(mutex);
            stopping = false;
        }
        changed.notify_all();
    }

    /** @copydoc ThreadPoolBackend::Configure() */
    void Configure(const size_t* newMinimum, const size_t* newMaximum) override {
        checkWorker();
        const std::scoped_lock lock(mutex);
        const size_t min = newMinimum ? *newMinimum : minimum;
        const size_t max = newMaximum ? *newMaximum : maximum;
        validate(min, max);
        minimum = min;
        maximum = max;
        if (state == ThreadPool::State::RUNNING) {
            grow(std::min(maximum, std::max(minimum, active + tasks.size())));
        }
        changed.notify_all();
    }

    /** @copydoc ThreadPoolBackend::Enqueue() */
    void Enqueue(std::function<void()> task) override {
        const std::scoped_lock lock(mutex);
        if (state == ThreadPool::State::RUNNING) {
            // Create the worker before publishing the task: a thread creation failure cannot strand it.
            grow(std::min(maximum, std::max(minimum, active + tasks.size() + 1)));
        }
        tasks.emplace(std::move(task));
        changed.notify_all();
    }

  private:
    /** Stable record of a worker thread and whether its scheduling loop has finished. */
    struct Worker {
        /** Owned thread handle, joined before this record is erased. */
        std::thread thread;
        /** Retirement flag protected by mutex; true once run() has finished its state updates. */
        bool done{false};
    };
    /**
     * Worker records, including retired handles waiting to be reclaimed.
     * std::list keeps the addresses borrowed by worker loops stable. Creation and
     * reaping hold mutex; Stop() holds lifecycleMutex and joins before clearing.
     **/
    std::list<Worker> workers;

    /**
     * Reclaims finished handles and creates workers until the target count is reached.
     * Existing live workers are retained when the target is below the current count.
     * Thread creation failure removes the unfinished record and propagates to the caller.
     * @param target Desired live worker count, no greater than the configured maximum.
     * @pre mutex is held and dispatch is RUNNING.
     **/
    void grow(size_t target) {
        // Reclaim retired threads instead of retaining every thread ever created until Stop().
        for (auto it = workers.begin(); it != workers.end();) {
            if (it->done) {
                it->thread.join();
                it = workers.erase(it);
            } else {
                ++it;
            }
        }
        while (current < target) {
            workers.emplace_back();
            Worker& worker = workers.back();
            try {
                worker.thread = std::thread([this, &worker] { run(worker); });
            } catch (...) {
                workers.pop_back();
                throw;
            }
            ++current;
        }
    }

    /**
     * Runs a worker's scheduling loop and updates its accounting when it retires.
     * Claims tasks under mutex and invokes them without that lock. Captures are
     * released before decrementing active, so Wait() includes task cleanup.
     * Retires on shutdown, a reduced maximum or an idle timeout above the minimum.
     * @param worker Stable record retained until this thread has been joined.
     **/
    void run(Worker& worker) {
        const WorkerScope workerScope(*this);
        std::unique_lock lock(mutex);
        while (state == ThreadPool::State::RUNNING) {
            if (current > maximum) {
                break;
            }
            if (!tasks.empty()) {
                auto task = std::move(tasks.front());
                tasks.pop();
                ++active;
                lock.unlock();
                task();    // packaged_task captures user exceptions.
                task = {}; // Release captures before reporting completion to Wait().
                lock.lock();
                --active;
                changed.notify_all();
                continue;
            }
            const bool ready = changed.wait_for(lock, THREAD_IDLE_TIMEOUT, [this] { return state == ThreadPool::State::STOP || !tasks.empty() || current > maximum; });
            // Retirement and submission use the same lock, preventing the last worker
            // from retiring after a submitter decided no new worker was needed.
            if (!ready && current > minimum) {
                break;
            }
        }
        --current;
        worker.done = true;
        changed.notify_all();
    }
};
} // namespace

std::shared_ptr<ThreadPoolBackend> MakeThreadPoolBackend(size_t minimum, size_t maximum) {
    return std::make_shared<BuiltinThreadPool>(minimum, maximum);
}
std::shared_ptr<ThreadPoolBackend> MakeThreadPoolBackend(boost::asio::io_context& /*context*/, size_t minimum, size_t maximum) {
    ThreadPoolBackend::validate(minimum, maximum);
    throw std::logic_error("A caller-owned Boost.Asio context requires CPR_THREAD_POOL=BOOST_ASIO");
}
} // namespace cpr
