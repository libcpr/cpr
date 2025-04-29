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

namespace cpr {
// NOLINTNEXTLINE(cert-err58-cpp) Not relevant since trivial function.
size_t ThreadPool::DEFAULT_MAX_THREAD_COUNT = std::max<size_t>(std::thread::hardware_concurrency(), static_cast<size_t>(1));

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
    assert(minThreadCount <= maxThreadCount);
    this->minThreadCount = minThreadCount;
}

void ThreadPool::SetMaxThreadCount(size_t maxThreadCount) {
    assert(minThreadCount <= maxThreadCount);
    this->maxThreadCount = maxThreadCount;
}

void ThreadPool::Start() {
    const std::unique_lock lock(controlMutex);
    if (setState(State::RUNNING)) {
        for (size_t i = 0; i < std::max(minThreadCount.load(), tasks.size()); i++) {
            addThread();
        }
    }
}

void ThreadPool::Stop() {
    const std::unique_lock controlLock(controlMutex);
    setState(State::STOP);
    taskQueueCondVar.notify_all();

    // Join all workers
    const std::unique_lock workersLock{workerMutex};
    auto iter = workers.begin();
    while (iter != workers.end()) {
        if (iter->thread->joinable()) {
            iter->thread->join();
        }
        iter = workers.erase(iter);
    }
}

void ThreadPool::Wait() const {
    while (true) {
        if ((state != State::RUNNING && curThreadCount <= 0) || (tasks.empty() && curThreadCount <= idleThreadCount)) {
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
    assert(state != State::STOP);

    const std::unique_lock lock{workerMutex};
    workers.emplace_back();
    workers.back().thread = std::make_unique<std::thread>(&ThreadPool::threadFunc, this, std::ref(workers.back()));
    curThreadCount++;
    idleThreadCount++;
}

void ThreadPool::threadFunc(WorkerThread& workerThread) {
    while (true) {
        std::cv_status result{std::cv_status::no_timeout};
        {
            std::unique_lock lock(taskQueueMutex);
            if (tasks.empty()) {
                result = taskQueueCondVar.wait_for(lock, std::chrono::milliseconds(250));
            }
        }

        if (state == State::STOP) {
            curThreadCount--;
            break;
        }

        // A timeout has been reached check if we should cleanup the thread
        if (result == std::cv_status::timeout) {
            const std::unique_lock lock(controlMutex);
            if (curThreadCount > minThreadCount) {
                curThreadCount--;
                break;
            }
        }

        // Check for tasks and execute one
        std::function<void()> task;
        {
            const std::unique_lock lock(taskQueueMutex);
            if (!tasks.empty()) {
                idleThreadCount--;
                task = std::move(tasks.front());
                tasks.pop();
            }
        }

        // Execute the task
        if (task) {
            task();
            idleThreadCount++;
        }
    }

    // Make sure we clean up other stopped threads
    if (state != State::STOP) {
        joinStoppedThreads();
    }

    workerThread.state = State::STOP;

    // Mark worker thread to be removed
    workerJoinReadyCount++;
    idleThreadCount--;
}

void ThreadPool::joinStoppedThreads() {
    const std::unique_lock lock{workerMutex};
    auto iter = workers.begin();
    while (iter != workers.end()) {
        if (iter->state == State::STOP) {
            if (iter->thread->joinable()) {
                iter->thread->join();
            }
            iter = workers.erase(iter);
            workerJoinReadyCount--;
        } else {
            iter++;
        }
    }
}
} // namespace cpr
