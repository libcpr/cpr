#pragma once

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
class ThreadPool2 {
  public:
    static constexpr size_t DEFAULT_MIN_THREAD_COUNT = 0;
    static size_t DEFAULT_MAX_THREAD_COUNT;

  private:
    enum class State : uint8_t { STOP, RUNNING };
    struct WorkerThread {
        std::unique_ptr<std::thread> thread{nullptr};
        State state{State::RUNNING};
    };

    std::mutex workerMutex;
    std::list<WorkerThread> workers;
    std::atomic_size_t workerJoinReadyCount{0};

    std::mutex taskQueueMutex;
    std::condition_variable taskQueueCondVar;
    std::queue<std::function<void()>> tasks;

    std::atomic<State> state = State::STOP;
    std::atomic_size_t minThreadCount;
    std::atomic_size_t curThreadCount{0};
    std::atomic_size_t maxThreadCount;
    std::atomic_size_t idleThreadCount{0};

    std::recursive_mutex controlMutex;

  public:
    explicit ThreadPool2(size_t minThreadCount = DEFAULT_MIN_THREAD_COUNT, size_t maxThreadCount = DEFAULT_MAX_THREAD_COUNT);
    ThreadPool2(const ThreadPool2& other) = delete;
    ThreadPool2(ThreadPool2&& old) = delete;
    ~ThreadPool2();

    ThreadPool2& operator=(const ThreadPool2& other) = delete;
    ThreadPool2& operator=(ThreadPool2&& old) = delete;

    [[nodiscard]] State GetState() const;
    [[nodiscard]] size_t GetMaxThreadCount() const;
    [[nodiscard]] size_t GetCurThreadCount() const;
    [[nodiscard]] size_t GetMinThreadCount() const;

    void SetMinThreadCount(size_t minThreadCount);
    void SetMaxThreadCount(size_t maxThreadCount);

    void Start();
    void Stop();
    void Wait();

    /**
     * Return a future, calling future.get() will wait task done and return RetType.
     * Submit(fn, args...)
     * Submit(std::bind(&Class::mem_fn, &obj))
     * Submit(std::mem_fn(&Class::mem_fn, &obj))
     **/
    template <class Fn, class... Args>
    auto Submit(Fn&& fn, Args&&... args) {
        // Add a new worker thread in case the tasks queue is not empty and we still can add a thread
        {
            std::unique_lock lock(taskQueueMutex);
            if (idleThreadCount < tasks.size() && curThreadCount < maxThreadCount) {
                addThread();
            }
        }

        // Add task to queue
        using RetType = decltype(fn(args...));
        const std::shared_ptr<std::packaged_task<RetType()>> task = std::make_shared<std::packaged_task<RetType()>>([fn = std::forward<Fn>(fn), args...]() mutable { return std::invoke(fn, args...); });
        std::future<RetType> future = task->get_future();
        {
            std::unique_lock lock(taskQueueMutex);
            tasks.emplace([task] { (*task)(); });
        }

        taskQueueCondVar.notify_one();
        return future;
    }

  private:
    void setState(State newState);
    void addThread();
    void joinStoppedThreads();

    void threadFunc(WorkerThread& workerThread);
};
} // namespace cpr
