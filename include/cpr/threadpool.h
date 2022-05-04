#ifndef CPR_THREAD_POOL_H
#define CPR_THREAD_POOL_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#define CPR_DEFAULT_THREAD_POOL_MIN_THREAD_NUM  1
#define CPR_DEFAULT_THREAD_POOL_MAX_THREAD_NUM  std::thread::hardware_concurrency()
#define CPR_DEFAULT_THREAD_POOL_MAX_IDLE_TIME   60000 // ms

namespace cpr {

class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool( int min_threads = CPR_DEFAULT_THREAD_POOL_MIN_THREAD_NUM,
                int max_threads = CPR_DEFAULT_THREAD_POOL_MAX_THREAD_NUM,
                int max_idle_ms = CPR_DEFAULT_THREAD_POOL_MAX_IDLE_TIME );

    virtual ~ThreadPool();

    void setMinThreadNum(int min_threads) {
        min_thread_num = min_threads;
    }
    void setMaxThreadNum(int max_threads) {
        max_thread_num = max_threads;
    }
    void setMaxIdleTime(int ms) {
        max_idle_time = ms;
    }
    int currentThreadNum() {
        return cur_thread_num;
    }
    int idleThreadNum() {
        return idle_thread_num;
    }
    bool isStarted() {
        return status != STOP;
    }
    bool isStopped() {
        return status == STOP;
    }

    int start(int start_threads = 0);
    int stop();
    int pause();
    int resume();
    int wait();

    /**
     * Return a future, calling future.get() will wait task done and return RetType.
     * commit(fn, args...)
     * commit(std::bind(&Class::mem_fn, &obj))
     * commit(std::mem_fn(&Class::mem_fn, &obj))
     **/
    template<class Fn, class... Args>
    auto commit(Fn&& fn, Args&&... args) -> std::future<decltype(fn(args...))> {
        if (status == STOP) { start(); }
        if (idle_thread_num == 0 && cur_thread_num < max_thread_num) {
            createThread();
        }
        using RetType = decltype(fn(args...));
        auto task = std::make_shared<std::packaged_task<RetType()> >(
            std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
        std::future<RetType> future = task->get_future();
        {
            std::lock_guard<std::mutex> locker(task_mutex);
            tasks.emplace([task]{
                (*task)();
            });
        }

        task_cond.notify_one();
        return future;
    }

private:
    bool createThread();
    void addThread(std::thread* thread);
    void delThread(std::thread::id id);

public:
    int min_thread_num;
    int max_thread_num;
    int max_idle_time;

private:
    enum Status {
        STOP,
        RUNNING,
        PAUSE,
    };
    struct ThreadData {
        std::shared_ptr<std::thread> thread;
        std::thread::id id;
        Status          status;
        time_t          start_time;
        time_t          stop_time;
    };
    std::atomic<Status>     status;
    std::atomic<int>        cur_thread_num;
    std::atomic<int>        idle_thread_num;
    std::list<ThreadData>   threads;
    std::mutex              thread_mutex;
    std::queue<Task>        tasks;
    std::mutex              task_mutex;
    std::condition_variable task_cond;
};

} // namespace cpr

#endif
