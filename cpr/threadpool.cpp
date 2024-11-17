#include "cpr/threadpool.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <ctime>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

namespace cpr {

ThreadPool::ThreadPool(size_t min_threads, size_t max_threads, std::chrono::milliseconds max_idle_ms) : min_thread_num(min_threads), max_thread_num(max_threads), max_idle_time(max_idle_ms) {}

ThreadPool::~ThreadPool() {
    Stop();
}

int ThreadPool::Start(size_t start_threads) {
    if (status != STOP) {
        return -1;
    }
    status = RUNNING;
    start_threads = std::clamp(start_threads, min_thread_num, max_thread_num);
    for (size_t i = 0; i < start_threads; ++i) {
        CreateThread();
    }
    return 0;
}

int ThreadPool::Stop() {
    const std::unique_lock status_lock(status_wait_mutex);
    if (status == STOP) {
        return -1;
    }

    status = STOP;
    status_wait_cond.notify_all();
    task_cond.notify_all();

    for (auto& i : threads) {
        if (i.thread->joinable()) {
            i.thread->join();
        }
    }

    threads.clear();
    cur_thread_num = 0;
    idle_thread_num = 0;
    return 0;
}

int ThreadPool::Pause() {
    if (status == RUNNING) {
        status = PAUSE;
    }
    return 0;
}

int ThreadPool::Resume() {
    const std::unique_lock status_lock(status_wait_mutex);
    if (status == PAUSE) {
        status = RUNNING;
        status_wait_cond.notify_all();
    }
    return 0;
}

int ThreadPool::Wait() {
    while (true) {
        if (status == STOP || (tasks.empty() && idle_thread_num == cur_thread_num)) {
            break;
        }
        std::this_thread::yield();
    }
    return 0;
}

bool ThreadPool::CreateThread() {
    if (cur_thread_num >= max_thread_num) {
        return false;
    }
    std::thread* thread = new std::thread([this] {
        bool initialRun = true;
        while (status != STOP) {
            {
                std::unique_lock status_lock(status_wait_mutex);
                status_wait_cond.wait(status_lock, [this]() { return status != Status::PAUSE; });
            }

            Task task;
            {
                std::unique_lock<std::mutex> locker(task_mutex);
                task_cond.wait_for(locker, std::chrono::milliseconds(max_idle_time), [this]() { return status == STOP || !tasks.empty(); });
                if (status == STOP) {
                    return;
                }
                if (tasks.empty()) {
                    if (cur_thread_num > min_thread_num) {
                        DelThread(std::this_thread::get_id());
                        return;
                    }
                    continue;
                }
                if (!initialRun) {
                    --idle_thread_num;
                }
                task = std::move(tasks.front());
                tasks.pop();
            }
            if (task) {
                task();
                ++idle_thread_num;
                if (initialRun) {
                    initialRun = false;
                }
            }
        }
    });
    AddThread(thread);
    return true;
}

void ThreadPool::AddThread(std::thread* thread) {
    thread_mutex.lock();
    ++cur_thread_num;
    ThreadData data;
    data.thread = std::shared_ptr<std::thread>(thread);
    data.id = thread->get_id();
    data.status = RUNNING;
    data.start_time = std::chrono::steady_clock::now();
    data.stop_time = std::chrono::steady_clock::time_point::max();
    threads.emplace_back(data);
    thread_mutex.unlock();
}

void ThreadPool::DelThread(std::thread::id id) {
    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    thread_mutex.lock();
    --cur_thread_num;
    --idle_thread_num;
    auto iter = threads.begin();
    while (iter != threads.end()) {
        if (iter->status == STOP && now > iter->stop_time) {
            if (iter->thread->joinable()) {
                iter->thread->join();
                iter = threads.erase(iter);
                continue;
            }
        } else if (iter->id == id) {
            iter->status = STOP;
            iter->stop_time = std::chrono::steady_clock::now();
        }
        ++iter;
    }
    thread_mutex.unlock();
}

} // namespace cpr
