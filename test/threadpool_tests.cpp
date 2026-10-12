#include "cpr/async.h"
#ifdef CPR_TEST_BOOST_ASIO
#include <boost/asio/io_context.hpp>
#endif
#include <atomic>
#include <chrono>
#include <cstddef>
#include <gtest/gtest.h>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

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
        // With minimum zero, an idle builtin worker may retire before these reads.
        EXPECT_LE(tp.GetCurThreadCount(), 1);
        EXPECT_LE(tp.GetIdleThreadCount(), 1);
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
        EXPECT_GE(tp.GetCurThreadCount(), 1);
        EXPECT_LE(tp.GetCurThreadCount(), 10);
        EXPECT_EQ(tp.GetIdleThreadCount(), tp.GetCurThreadCount());
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

namespace {
using namespace std::chrono_literals;

template <typename Predicate>
bool waitUntil(Predicate predicate) {
    const auto deadline = std::chrono::steady_clock::now() + 5s;
    while (!predicate()) {
        if (std::chrono::steady_clock::now() >= deadline) {
            return false;
        }
        std::this_thread::yield();
    }
    return true;
}

void testCancellation(size_t workerCount) {
    cpr::ThreadPool tp(1, workerCount);
    std::promise<void> release;
    auto gate = release.get_future().share();
    std::atomic_size_t started{0};
    std::vector<std::future<void>> running;
    for (size_t i = 0; i < workerCount; ++i) {
        running.push_back(tp.Submit([&started, gate] {
            ++started;
            gate.wait();
        }));
    }
    const bool allStarted = waitUntil([&] { return started == workerCount; });
    EXPECT_TRUE(allStarted);
    auto canceled = tp.Submit([] {});
    auto stopping = std::async(std::launch::async, [&] { tp.Stop(); });
    EXPECT_TRUE(waitUntil([&] { return tp.GetState() == cpr::ThreadPool::State::STOP; }));
    EXPECT_EQ(canceled.wait_for(5s), std::future_status::ready);
    release.set_value();
    stopping.get();
    for (auto& future : running) {
        EXPECT_NO_THROW(future.get());
    }
    try {
        canceled.get();
        FAIL() << "Queued work must be canceled";
    } catch (const std::future_error& error) {
        EXPECT_EQ(error.code(), std::make_error_code(std::future_errc::broken_promise));
    }
    EXPECT_EQ(tp.GetCurThreadCount(), 0);
    EXPECT_EQ(tp.GetIdleThreadCount(), 0);
}
} // namespace

TEST(ThreadPoolTests, CanceledBeforeDoneSingleThread) {
    testCancellation(1);
}
TEST(ThreadPoolTests, CanceledBeforeDoneMultipleThreads) {
    testCancellation(10);
}

TEST(ThreadPoolTests, MoveOnlyArgumentsAndCallable) {
    cpr::ThreadPool tp(0, 1);
    auto result = tp.Submit([offset = std::make_unique<int>(2)](std::unique_ptr<int> value) { return *offset + *value; }, std::make_unique<int>(40));
    EXPECT_EQ(result.get(), 42);
    struct Object {
        int value{17};
    } object;
    EXPECT_EQ(tp.Submit(&Object::value, &object).get(), 17);
}

TEST(ThreadPoolTests, ExceptionsPropagateAndWorkerSurvives) {
    cpr::ThreadPool tp(0, 1);
    auto result = tp.Submit([]() -> int { throw std::runtime_error("task failure"); });
    EXPECT_THROW(result.get(), std::runtime_error);
    EXPECT_EQ(tp.Submit([] { return 42; }).get(), 42);
}

TEST(ThreadPoolTests, ReferenceArgumentsAndVoidResult) {
    cpr::ThreadPool tp(0, 1);
    int value{0};
    auto result = tp.Submit([](int& target) { target = 42; }, std::ref(value));
    result.get();
    EXPECT_EQ(value, 42);
}

TEST(ThreadPoolTests, ResizingPreservesQueuedTasks) {
    cpr::ThreadPool tp(0, 1);
    std::promise<void> started;
    std::promise<void> release;
    auto gate = release.get_future().share();
    auto running = tp.Submit([&] {
        started.set_value();
        gate.wait();
    });
    started.get_future().wait();
    auto queued = tp.Submit([] { return 42; });
    auto resizing = std::async(std::launch::async, [&] { tp.SetThreadCounts(0, 2); });
#ifdef CPR_TEST_BOOST_ASIO
    EXPECT_TRUE(waitUntil([&] { return tp.GetState() == cpr::ThreadPool::State::STOP; }));
    EXPECT_EQ(resizing.wait_for(0ms), std::future_status::timeout);
#endif
    release.set_value();
    running.get();
    resizing.get();
    EXPECT_EQ(queued.get(), 42);
    tp.Wait();
    EXPECT_EQ(tp.GetMaxThreadCount(), 2);
}

TEST(ThreadPoolTests, InvalidLimitsLeaveConfigurationUnchanged) {
    EXPECT_THROW((cpr::ThreadPool{0, 0}), std::invalid_argument);
    EXPECT_THROW((cpr::ThreadPool{2, 1}), std::invalid_argument);
    cpr::ThreadPool tp(1, 2);
    EXPECT_THROW(tp.SetMinThreadCount(3), std::invalid_argument);
    EXPECT_THROW(tp.SetMaxThreadCount(0), std::invalid_argument);
    EXPECT_THROW(tp.SetThreadCounts(3, 2), std::invalid_argument);
    EXPECT_EQ(tp.GetMinThreadCount(), 1);
    EXPECT_EQ(tp.GetMaxThreadCount(), 2);
    tp.SetThreadCounts(3, 3);
    EXPECT_EQ(tp.GetMinThreadCount(), 3);
    EXPECT_EQ(tp.GetMaxThreadCount(), 3);
    EXPECT_EQ(tp.GetCurThreadCount(), 3);
    tp.SetThreadCounts(0, 1);
    tp.Wait();
    EXPECT_TRUE(waitUntil([&] { return tp.GetCurThreadCount() <= 1; }));
    EXPECT_EQ(tp.Submit([] { return 42; }).get(), 42);
}

TEST(ThreadPoolTests, QueuedWhileStoppedRunsAfterRestart) {
    cpr::ThreadPool tp(0, 2);
    tp.Stop();
    auto result = tp.Submit([] { return 42; });
    EXPECT_EQ(result.wait_for(0ms), std::future_status::timeout);
    tp.Wait(); // Stopped pools do not wait for work queued for the next Start().
    tp.Start();
    EXPECT_EQ(result.get(), 42);
    tp.Wait();
    tp.Stop();
    tp.Stop();
    tp.Start();
    tp.Start();
    EXPECT_EQ(tp.Submit([] { return 17; }).get(), 17);
}

TEST(ThreadPoolTests, WaitIncludesActiveTasksAndTheirSubmissions) {
    cpr::ThreadPool tp(0, 1);
    std::promise<void> started;
    std::promise<void> release;
    auto gate = release.get_future().share();
    std::atomic_bool nestedDone{false};
    tp.Submit([&] {
        started.set_value();
        gate.wait();
        tp.Submit([&] { nestedDone = true; });
    });
    started.get_future().wait();
    auto waiting = std::async(std::launch::async, [&] { tp.Wait(); });
    EXPECT_EQ(waiting.wait_for(20ms), std::future_status::timeout);
    release.set_value();
    waiting.get();
    EXPECT_TRUE(nestedDone);
}

TEST(ThreadPoolTests, WorkerLifecycleCallsFailInsteadOfDeadlocking) {
    cpr::ThreadPool tp(0, 1);
    auto result = tp.Submit([&] {
        EXPECT_THROW(tp.Wait(), std::logic_error);
        EXPECT_THROW(tp.Stop(), std::logic_error);
        EXPECT_THROW(tp.Start(), std::logic_error);
        EXPECT_THROW(tp.SetMinThreadCount(0), std::logic_error);
        EXPECT_THROW(tp.SetMaxThreadCount(1), std::logic_error);
        EXPECT_THROW(tp.SetThreadCounts(0, 1), std::logic_error);
    });
    EXPECT_NO_THROW(result.get());
    EXPECT_EQ(tp.Submit([] { return 42; }).get(), 42);
}

TEST(ThreadPoolTests, ConcurrentProducersRespectMaximum) {
    cpr::ThreadPool tp(0, 3);
    std::atomic_size_t executing{0};
    std::atomic_size_t completed{0};
    std::atomic_size_t peak{0};
    std::vector<std::thread> producers;
    for (size_t i = 0; i < 8; ++i) {
        producers.emplace_back([&] {
            for (size_t j = 0; j < 100; ++j) {
                tp.Submit([&] {
                    const auto count = ++executing;
                    auto previous = peak.load();
                    while (previous < count && !peak.compare_exchange_weak(previous, count)) {
                    }
                    std::this_thread::yield();
                    --executing;
                    ++completed;
                });
            }
        });
    }
    for (auto& producer : producers) {
        producer.join();
    }
    tp.Wait();
    EXPECT_EQ(completed, 800);
    EXPECT_LE(peak, 3);
}

TEST(ThreadPoolTests, ConcurrentStartStopAndSubmit) {
    cpr::ThreadPool tp(0, 2);
    std::vector<std::thread> controllers;
    for (size_t i = 0; i < 3; ++i) {
        controllers.emplace_back([&] {
            for (size_t j = 0; j < 30; ++j) {
                tp.Stop();
                tp.Start();
                tp.Submit([] {});
            }
        });
    }
    for (auto& controller : controllers) {
        controller.join();
    }
    tp.Start();
    EXPECT_EQ(tp.Submit([] { return 42; }).get(), 42);
    tp.Wait();
    EXPECT_LE(tp.GetCurThreadCount(), 2);
}

TEST(ThreadPoolTests, IdleBackendBehaviorAndResume) {
    cpr::ThreadPool tp(0, 2);
    EXPECT_EQ(tp.Submit([] { return 42; }).get(), 42);
    tp.Wait();
#ifdef CPR_TEST_BOOST_ASIO
    EXPECT_EQ(tp.GetCurThreadCount(), 2);
    EXPECT_EQ(tp.GetIdleThreadCount(), 2);
#else
    EXPECT_TRUE(waitUntil([&] { return tp.GetCurThreadCount() == 0; }));
    EXPECT_EQ(tp.GetIdleThreadCount(), 0);
#endif
    EXPECT_EQ(tp.Submit([] { return 17; }).get(), 17);
}

TEST(ThreadPoolTests, DefaultMaximumCanBeConfiguredByConsumer) {
    const auto originalMaximum = cpr::ThreadPool::DEFAULT_MAX_THREAD_COUNT;
    cpr::ThreadPool::DEFAULT_MAX_THREAD_COUNT = 1;
    {
        cpr::ThreadPool pool;
        EXPECT_EQ(pool.GetMaxThreadCount(), 1);
        EXPECT_EQ(pool.Submit([] { return 42; }).get(), 42);
    }
    cpr::ThreadPool::DEFAULT_MAX_THREAD_COUNT = originalMaximum;
}

TEST(ThreadPoolTests, StartupChangesBothLimitsAtomically) {
    cpr::async::startup(1, 1);
#ifdef CPR_TEST_BOOST_ASIO
    boost::asio::io_context context;
    EXPECT_THROW(cpr::GlobalThreadPool::GetInstance(context), std::logic_error);
#endif
    cpr::async::startup(3, 3); // Minimum exceeds the old maximum.
    auto* pool = cpr::GlobalThreadPool::GetInstance();
    EXPECT_EQ(pool->GetMinThreadCount(), 3);
    EXPECT_EQ(pool->GetMaxThreadCount(), 3);
    EXPECT_EQ(cpr::async([] { return 42; }).get(), 42);
    cpr::async::startup();
    cpr::async::cleanup();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
