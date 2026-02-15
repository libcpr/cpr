#include "cpr/threadpool.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace cpr {
// NOLINTNEXTLINE(cert-err58-cpp) Not relevant since trivial function.
size_t ThreadPool::DEFAULT_MAX_THREAD_COUNT = std::max<size_t>(std::thread::hardware_concurrency(), static_cast<size_t>(1));

namespace {
constexpr std::chrono::milliseconds THREAD_IDLE_TIMEOUT{250};
} // namespace

ThreadPool::ThreadPool(size_t minThreadCount, size_t maxThreadCount) : minThreadCount(minThreadCount), maxThreadCount(maxThreadCount) {
    assert(minThreadCount <= maxThreadCount);
    Start();
}

ThreadPool::~ThreadPool() {
    Stop();
}

ThreadPool::State ThreadPool::GetState() const {
    return state.load();
}

size_t ThreadPool::GetMaxThreadCount() const {
    return maxThreadCount.load();
}

size_t ThreadPool::GetCurThreadCount() const {
    return curThreadCount.load();
}

size_t ThreadPool::GetIdleThreadCount() const {
    return idleThreadCount.load();
}

size_t ThreadPool::GetMinThreadCount() const {
    return minThreadCount.load();
}

void ThreadPool::SetMinThreadCount(size_t minThreadCount) {
    const std::unique_lock lock(controlMutex);
    assert(minThreadCount <= maxThreadCount.load());
    this->minThreadCount = minThreadCount;

    if (state == State::RUNNING) {
        while (curThreadCount < this->minThreadCount) {
            addThread();
        }
    }
}

void ThreadPool::SetMaxThreadCount(size_t maxThreadCount) {
    const std::unique_lock lock(controlMutex);
    assert(minThreadCount.load() <= maxThreadCount);
    this->maxThreadCount = maxThreadCount;
}

void ThreadPool::Start() {
    const std::unique_lock lock(controlMutex);
    if (setState(State::RUNNING)) {
        size_t taskCount{0};
        {
            const std::unique_lock queueLock(taskQueueMutex);
            taskCount = tasks.size();
        }

        const size_t targetThreadCount = std::min(maxThreadCount.load(), std::max(minThreadCount.load(), taskCount));
        while (curThreadCount < targetThreadCount) {
            addThread();
        }
    }
}

void ThreadPool::Stop() {
    {
        const std::unique_lock controlLock(controlMutex);
        setState(State::STOP);
    }
    taskQueueCondVar.notify_all();

    std::vector<std::unique_ptr<std::thread>> workersToJoin;
    {
        const std::unique_lock workersLock{workerMutex};
        workersToJoin.reserve(workers.size());
        for (auto& worker : workers) {
            workersToJoin.emplace_back(std::move(worker.thread));
        }
        workers.clear();
    }

    for (auto& worker : workersToJoin) {
        if (worker != nullptr && worker->joinable()) {
            worker->join();
        }
    }

    {
        const std::unique_lock queueLock(taskQueueMutex);
        std::queue<std::function<void()>> emptyTasks;
        tasks.swap(emptyTasks);
    }

    curThreadCount = 0;
    idleThreadCount = 0;
}

void ThreadPool::Wait() const {
    while (true) {
        const State currentState = state.load();
        const size_t currentThreadCount = curThreadCount.load();
        const size_t currentIdleThreadCount = idleThreadCount.load();

        bool hasPendingTasks{false};
        {
            const std::unique_lock queueLock(taskQueueMutex);
            hasPendingTasks = !tasks.empty();
        }

        if (currentState != State::RUNNING) {
            if (currentThreadCount == 0) {
                break;
            }
        } else if (!hasPendingTasks && currentThreadCount <= currentIdleThreadCount) {
            break;
        }

        std::this_thread::yield();
    }
}

bool ThreadPool::setState(State state) {
    const std::unique_lock lock(controlMutex);
    if (this->state == state) {
        return false;
    }
    this->state = state;
    return true;
}

void ThreadPool::addThread() {
    const std::unique_lock controlLock(controlMutex);
    if (state == State::STOP || curThreadCount >= maxThreadCount) {
        return;
    }

    const std::unique_lock lock{workerMutex};
    workers.emplace_back();
    workers.back().thread = std::make_unique<std::thread>(&ThreadPool::threadFunc, this);
    curThreadCount++;
    idleThreadCount++;
}

void ThreadPool::threadFunc() {
    while (true) {
        std::function<void()> task;
        bool waitTimedOut{false};
        {
            std::unique_lock queueLock(taskQueueMutex);

            const bool shouldContinue = taskQueueCondVar.wait_for(queueLock, THREAD_IDLE_TIMEOUT, [this]() { return state == State::STOP || !tasks.empty(); });
            waitTimedOut = !shouldContinue;

            if (state == State::STOP) {
                break;
            }

            if (!waitTimedOut && !tasks.empty()) {
                idleThreadCount--;
                task = std::move(tasks.front());
                tasks.pop();
            }
        }

        if (waitTimedOut) {
            const std::unique_lock lock(controlMutex);
            if (curThreadCount > minThreadCount) {
                curThreadCount--;
                idleThreadCount--;
                return;
            }
            continue;
        }

        if (task) {
            task();
            idleThreadCount++;
        }
    }

    curThreadCount--;
    idleThreadCount--;
}
} // namespace cpr
