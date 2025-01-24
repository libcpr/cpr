#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

namespace cpr {
class ThreadPool2 {
  public:
    static constexpr size_t DEFAULT_MIN_THREAD_COUNT = 0;
    static size_t DEFAULT_MAX_THREAD_COUNT;

  private:
    enum class State : uint8_t { STOP, RUNNING, PAUSE };
    struct WorkerThread {
        std::unique_ptr<std::thread> thread{nullptr};
        State state{State::RUNNING};
    };

    std::mutex workerMutex;
    std::list<WorkerThread> workers;
    std::atomic_size_t workerJoinReadyCount{0};

    std::mutex taskQueueMutex;
    std::condition_variable taskQueueCondVar;

    std::atomic<State> state = State::STOP;
    std::atomic_size_t minThreadCount;
    std::atomic_size_t curThreadCount{0};
    std::atomic_size_t maxThreadCount;

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

  private:
    void setState(State newState);
    void addThread();
    void joinStoppedThreads();

    void threadFunc(WorkerThread& workerThread);
};
} // namespace cpr
