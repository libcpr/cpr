#include <atomic>
#include <cstddef>
#include <gtest/gtest.h>
#include <mutex>

#include "cpr/threadpool.h"

TEST(ThreadPoolTests, BasicWorkOneThread) {
    std::atomic_uint32_t invCount{0};
    uint32_t invCountExpected{100};

    {
        cpr::ThreadPool tp(0, 1);

        for (size_t i = 0; i < invCountExpected; ++i) {
            tp.Submit([&invCount]() -> void { invCount++; });
        }

        // Wait for the thread pool to finish its work
        tp.Wait();
        EXPECT_EQ(tp.GetCurThreadCount(), 1);
        EXPECT_EQ(tp.GetIdleThreadCount(), 1);
        EXPECT_EQ(tp.GetMaxThreadCount(), 1);
        EXPECT_EQ(tp.GetMinThreadCount(), 0);
    }

    EXPECT_EQ(invCount, invCountExpected);
}

TEST(ThreadPoolTests, BasicWorkOneMinThread) {
    std::atomic_uint32_t invCount{0};
    uint32_t invCountExpected{100};

    {
        cpr::ThreadPool tp(1, 1);

        for (size_t i = 0; i < invCountExpected; ++i) {
            tp.Submit([&invCount]() -> void { invCount++; });
        }

        // Wait for the thread pool to finish its work
        tp.Wait();
        EXPECT_EQ(tp.GetCurThreadCount(), 1);
        EXPECT_EQ(tp.GetIdleThreadCount(), 1);
        EXPECT_EQ(tp.GetMaxThreadCount(), 1);
        EXPECT_EQ(tp.GetMinThreadCount(), 1);
    }

    EXPECT_EQ(invCount, invCountExpected);
}

TEST(ThreadPoolTests, BasicWorkMultipleThreads) {
    std::atomic_uint32_t invCount{0};
    uint32_t invCountExpected{100};

    {
        cpr::ThreadPool tp(1, 10);

        for (size_t i = 0; i < invCountExpected; ++i) {
            tp.Submit([&invCount]() -> void { invCount++; });
        }

        // Wait for the thread pool to finish its work
        tp.Wait();
        EXPECT_GE(tp.GetCurThreadCount(), 1);
        EXPECT_LE(tp.GetCurThreadCount(), 10);

        EXPECT_GE(tp.GetIdleThreadCount(), 1);
        EXPECT_LE(tp.GetIdleThreadCount(), 10);

        EXPECT_EQ(tp.GetMaxThreadCount(), 10);
        EXPECT_EQ(tp.GetMinThreadCount(), 1);
    }

    EXPECT_EQ(invCount, invCountExpected);
}

TEST(ThreadPoolTests, StartStopBasicWorkMultipleThreads) {
    uint32_t invCountExpected{100};

    cpr::ThreadPool tp(1, 10);

    for (size_t i = 0; i < 100; i++) {
        std::atomic_uint32_t invCount{0};
        tp.Start();
        EXPECT_EQ(tp.GetCurThreadCount(), 1);
        EXPECT_EQ(tp.GetIdleThreadCount(), 1);
        EXPECT_EQ(tp.GetMaxThreadCount(), 10);
        EXPECT_EQ(tp.GetMinThreadCount(), 1);

        {
            for (size_t i = 0; i < invCountExpected; ++i) {
                tp.Submit([&invCount]() -> void { invCount++; });
            }

            // Wait for the thread pool to finish its work
            tp.Wait();
            EXPECT_GE(tp.GetCurThreadCount(), 1);
            EXPECT_LE(tp.GetCurThreadCount(), 10);

            EXPECT_GE(tp.GetIdleThreadCount(), 1);
            EXPECT_LE(tp.GetIdleThreadCount(), 10);

            EXPECT_EQ(tp.GetMaxThreadCount(), 10);
            EXPECT_EQ(tp.GetMinThreadCount(), 1);
        }

        EXPECT_EQ(invCount, invCountExpected);
        tp.Stop();

        EXPECT_EQ(tp.GetCurThreadCount(), 0);
        EXPECT_EQ(tp.GetIdleThreadCount(), 0);
        EXPECT_EQ(tp.GetMaxThreadCount(), 10);
        EXPECT_EQ(tp.GetMinThreadCount(), 1);
    }
}

// Ensure only the current task gets finished when stopping worker
TEST(ThreadPoolTests, CanceledBeforeDoneSingleThread) {
    std::atomic_uint32_t threadsDone{0};
    std::atomic_uint32_t threadsWaiting{0};
    std::mutex lock;
    lock.lock();

    {
        cpr::ThreadPool tp(1, 1);

        for (size_t i = 0; i < 100; ++i) {
            tp.Submit([&threadsDone, &lock, &threadsWaiting]() -> void {
                threadsWaiting++;
                const std::unique_lock guard(lock);
                threadsDone++;
            });
        }

        // Wait until all threads started. Can be replaced by std::barrier in C++20.
        while (threadsWaiting < 1) {
            std::this_thread::yield();
        }

        EXPECT_EQ(tp.GetCurThreadCount(), 1);
        EXPECT_EQ(tp.GetIdleThreadCount(), 0);
        EXPECT_EQ(tp.GetMaxThreadCount(), 1);
        EXPECT_EQ(tp.GetMinThreadCount(), 1);

        lock.unlock();
    }

    EXPECT_EQ(threadsDone, 1);
}

// Ensure only the current task gets finished when stopping worker
TEST(ThreadPoolTests, CanceledBeforeDoneMultipleThreads) {
    std::atomic_uint32_t threadsDone{0};
    std::atomic_uint32_t threadsWaiting{0};
    std::mutex lock;
    lock.lock();

    {
        cpr::ThreadPool tp(1, 10);

        for (size_t i = 0; i < 100; ++i) {
            tp.Submit([&threadsDone, &lock, &threadsWaiting]() -> void {
                threadsWaiting++;
                const std::unique_lock guard(lock);
                threadsDone++;
            });
        }

        // Wait until all threads started. Can be replaced by std::barrier in C++20.
        while (threadsWaiting < 10) {
            std::this_thread::yield();
        }

        EXPECT_EQ(threadsDone, 0);

        EXPECT_EQ(tp.GetCurThreadCount(), 10);
        EXPECT_EQ(tp.GetIdleThreadCount(), 0);
        EXPECT_EQ(tp.GetMaxThreadCount(), 10);
        EXPECT_EQ(tp.GetMinThreadCount(), 1);

        lock.unlock();
    }

    EXPECT_EQ(threadsDone, 10);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
