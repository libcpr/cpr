#include "cpr/threadpool2.h"
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
size_t ThreadPool2::DEFAULT_MAX_THREAD_COUNT = std::thread::hardware_concurrency();

ThreadPool2::ThreadPool2(size_t minThreadCount, size_t maxThreadCount) : minThreadCount(minThreadCount), maxThreadCount(maxThreadCount) {
    assert(minThreadCount <= maxThreadCount);
    Start();
}

ThreadPool2::~ThreadPool2() {
    Stop();
}

ThreadPool2::State ThreadPool2::GetState() const {
    return state.load();
}

size_t ThreadPool2::GetMaxThreadCount() const {
    return maxThreadCount.load();
}

size_t ThreadPool2::GetCurThreadCount() const {
    return curThreadCount.load();
}

size_t ThreadPool2::GetMinThreadCount() const {
    return minThreadCount.load();
}

void ThreadPool2::SetMinThreadCount(size_t minThreadCount) {
    assert(minThreadCount <= maxThreadCount);
    this->minThreadCount = minThreadCount;
}

void ThreadPool2::SetMaxThreadCount(size_t maxThreadCount) {
    assert(minThreadCount <= maxThreadCount);
    this->maxThreadCount = maxThreadCount;
}

void ThreadPool2::Start() {
    const std::unique_lock lock(controlMutex);
    setState(State::RUNNING);
}

void ThreadPool2::Stop() {
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

void ThreadPool2::Wait() {
    while (true) {
        if ((state != State::RUNNING && curThreadCount <= 0) || (tasks.empty() && curThreadCount <= idleThreadCount)) {
            break;
        }
        std::this_thread::yield();
    }
}

void ThreadPool2::setState(State state) {
    const std::unique_lock lock(controlMutex);
    if (this->state == state) {
        return;
    }
    this->state = state;
}

void ThreadPool2::addThread() {
    assert(state != State::STOP);

    const std::unique_lock lock{workerMutex};
    workers.emplace_back();
    workers.back().thread = std::make_unique<std::thread>(&ThreadPool2::threadFunc, this, std::ref(workers.back()));
    curThreadCount++;
    idleThreadCount++;
}

void ThreadPool2::threadFunc(WorkerThread& workerThread) {
    while (true) {
        std::cv_status result{std::cv_status::timeout};
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
        const std::unique_lock lock(taskQueueMutex);
        if (!tasks.empty()) {
            idleThreadCount--;
            const std::function<void()> task = std::move(tasks.front());
            tasks.pop();

            // Execute the task
            task();
        }
        idleThreadCount++;
    }

    workerThread.state = State::STOP;

    // Mark worker thread to be removed
    workerJoinReadyCount++;
}

void ThreadPool2::joinStoppedThreads() {
    const std::unique_lock lock{workerMutex};
    auto iter = workers.begin();
    while (iter != workers.end()) {
        if (iter->state == State::STOP) {
            if (iter->thread->joinable()) {
                iter->thread->join();
            }
            iter = workers.erase(iter);
            workerJoinReadyCount--;
        }
    }
}
} // namespace cpr
