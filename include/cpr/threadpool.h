#ifndef CPR_THREADPOOL_H
#define CPR_THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace cpr {
/**
 * cpr thread pool implementation used by async requests.
 *
 * Example:
 * // Create a new thread pool object
 * cpr::ThreadPool tp;
 * // Add work
 * tp.Submit(..)
 * ...
 * // Stop/join workers and flush the task queue
 * tp.Stop()
 * // Start the thread pool again spawning the initial set of worker threads.
 * tp.Start()
 * ...
 **/
class ThreadPool {
  public:
    /**
     * The default minimum thread count for the thread pool.
     * Even if there is no work this number of threads should be in standby for once new work arrives.
     **/
    static constexpr size_t DEFAULT_MIN_THREAD_COUNT = 0;
    /**
     * The default maximum thread count for the thread pool.
     * Even if there is a lot of work, the thread pool is not allowed to create more threads than this number.
     **/
    static size_t DEFAULT_MAX_THREAD_COUNT;

  private:
    /**
     * The thread pool or worker thread state.
     **/
    enum class State : uint8_t { STOP, RUNNING };
    /**
     * Collection of properties identifying a worker thread for the thread pool.
     **/
    struct WorkerThread {
        std::unique_ptr<std::thread> thread{nullptr};
    };

    /**
     * Mutex for synchronizing access to the worker thread list.
     **/
    std::mutex workerMutex;
    /**
     * A list of all worker threads
     **/
    std::list<WorkerThread> workers;
    /**
     * Mutex for synchronizing access to the task queue.
     **/
    mutable std::mutex taskQueueMutex;
    /**
     * Conditional variable to let threads wait for new work to arrive.
     **/
    std::condition_variable taskQueueCondVar;
    /**
     * A queue of tasks synchronized by 'taskQueueMutex'.
     **/
    std::queue<std::function<void()>> tasks;

    /**
     * The current state for the thread pool.
     **/
    std::atomic<State> state = State::STOP;
    /**
     * The number of threads that should always be in standby or working.
     **/
    std::atomic_size_t minThreadCount;
    /**
     * The current number of threads available to the thread pool (working or idle).
     **/
    std::atomic_size_t curThreadCount{0};
    /**
     * The maximum number of threads allowed to be used by this thread pool.
     **/
    std::atomic_size_t maxThreadCount;
    /**
     * The number of idle threads without any work awaiting new tasks.
     **/
    std::atomic_size_t idleThreadCount{0};

    /**
     * General control mutex synchronizing access to internal thread pool resources.
     **/
    std::recursive_mutex controlMutex;

  public:
    /**
     * Creates a new thread pool object with a minimum and maximum thread count.
     * Starts the thread pool via spawning 'minThreadCount' threads.
     * minThreadCount: Number of threads that should always be in standby or working.
     * maxThreadCount: The maximum number of threads allowed to be used by this thread pool.
     **/
    explicit ThreadPool(size_t minThreadCount = DEFAULT_MIN_THREAD_COUNT, size_t maxThreadCount = DEFAULT_MAX_THREAD_COUNT);
    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool(ThreadPool&& old) = delete;
    virtual ~ThreadPool();

    ThreadPool& operator=(const ThreadPool& other) = delete;
    ThreadPool& operator=(ThreadPool&& old) = delete;

    /**
     * Returns the current thread pool state.
     * The thread pool is in RUNNING state when initially created and will move over to STOP once Stop() is invoked.
     **/
    [[nodiscard]] State GetState() const;
    /**
     * Returns the maximum number of threads allowed to be used by this thread pool.
     **/
    [[nodiscard]] size_t GetMaxThreadCount() const;
    /**
     * Returns the current number of threads available to the thread pool (working or idle).
     **/
    [[nodiscard]] size_t GetCurThreadCount() const;
    /**
     * Returns the number of idle threads without any work awaiting new tasks.
     **/
    [[nodiscard]] size_t GetIdleThreadCount() const;
    /**
     * Returns the number of threads that should always be in standby or working.
     **/
    [[nodiscard]] size_t GetMinThreadCount() const;

    /**
     * Sets the number of threads that should always be in standby or working.
     **/
    void SetMinThreadCount(size_t minThreadCount);
    /**
     * Sets the maximum number of threads allowed to be used by this thread pool.
     **/
    void SetMaxThreadCount(size_t maxThreadCount);

    /**
     * Starts the thread pool by spawning GetMinThreadCount() threads.
     * Does nothing in case the thread pool state is already RUNNING.
     **/
    void Start();
    /**
     * Sets the state to STOP, clears the task queue and joins all running threads.
     * This means waiting for all threads that currently work on something letting them finish their task.
     **/
    void Stop();
    /**
     * Returns as soon as the task queue is empty and all threads are either stopped/joined or in idel state.
     **/
    void Wait() const;

    /**
     * Enqueues a new task to the thread pool.
     * Return a future, calling future.get() will wait task done and return RetType.
     * Submit(fn, args...)
     * Submit(std::bind(&Class::mem_fn, &obj))
     * Submit(std::mem_fn(&Class::mem_fn, &obj))
     *
     * Will start a new thread in case all other threads are currently working and GetCurThreadCount() < GetMaxThreadCount().
     **/
    template <class Fn, class... Args>
    auto Submit(Fn&& fn, Args&&... args) {
        // Add task to queue
        using RetType = decltype(fn(args...));
        const std::shared_ptr<std::packaged_task<RetType()>> task =
                std::make_shared<std::packaged_task<RetType()>>([fn = std::forward<Fn>(fn), args...]() mutable { return std::invoke(fn, args...); });
        std::future<RetType> future = task->get_future();

        {
            const std::unique_lock lockControl(controlMutex);
            size_t queueSize{0};
            {
                std::unique_lock lockQueue(taskQueueMutex);
                tasks.emplace([task] { (*task)(); });
                queueSize = tasks.size();
            }

            // Add a new worker thread if queued tasks exceed idle workers and we can still scale out.
            bool shouldAddThread{false};
            if (state == State::RUNNING && idleThreadCount < queueSize && curThreadCount < maxThreadCount) {
                shouldAddThread = true;
            }

            if (shouldAddThread) {
                addThread();
            }
        }

        taskQueueCondVar.notify_one();
        return future;
    }

  private:
    /**
     * Sets the new thread pool state.
     * Returns true in case the current state was different to the newState.
     **/
    bool setState(State newState);
    /**
     * Adds a new worker thread.
     **/
    void addThread();

    /**
     * The thread entry point where the heavy lifting happens.
     **/
    void threadFunc();
};
} // namespace cpr
#endif
