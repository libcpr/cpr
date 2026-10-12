#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <gtest/gtest.h>

#include "cpr/async.h"

namespace {
using namespace std::chrono_literals;

// Keep the caller's event loop running until each test has finished its cpr work.
class ContextRunner {
  public:
    explicit ContextRunner(boost::asio::io_context& context, size_t count = 1) : guard(boost::asio::make_work_guard(context)) {
        for (size_t i = 0; i < count; ++i) {
            threads.emplace_back([&context] { context.run(); });
        }
    }
    ~ContextRunner() {
        guard.reset();
        for (auto& thread : threads) {
            thread.join();
        }
    }

  private:
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard;
    std::vector<std::thread> threads;
};
} // namespace

TEST(AsioContextTests, UsesCallerEventLoopAndPropagatesResults) {
    boost::asio::io_context context;
    cpr::ThreadPool pool(context, 0, 1);
    EXPECT_EQ(pool.GetCurThreadCount(), 0);
    auto result = pool.Submit([](std::unique_ptr<int> value) { return *value; }, std::make_unique<int>(42));
    EXPECT_EQ(result.wait_for(0ms), std::future_status::timeout);
    auto failure = pool.Submit([] { throw std::runtime_error("task failure"); });
    context.run();
    EXPECT_EQ(result.get(), 42);
    EXPECT_THROW(failure.get(), std::runtime_error);
    pool.Wait();
    EXPECT_EQ(pool.GetCurThreadCount(), 0);
    EXPECT_EQ(pool.GetIdleThreadCount(), 0);
}

TEST(AsioContextTests, InvalidLimitsDoNotChangeOrStopContext) {
    boost::asio::io_context context;
    EXPECT_THROW((cpr::ThreadPool{context, 0, 0}), std::invalid_argument);
    EXPECT_THROW((cpr::ThreadPool{context, 2, 1}), std::invalid_argument);
    cpr::ThreadPool pool(context, 0, 2);
    EXPECT_THROW(pool.SetThreadCounts(3, 2), std::invalid_argument);
    EXPECT_EQ(pool.GetMinThreadCount(), 0);
    EXPECT_EQ(pool.GetMaxThreadCount(), 2);
    EXPECT_FALSE(context.stopped());
}

TEST(AsioContextTests, StopCancelsPendingWorkAndPreservesUnrelatedHandlers) {
    boost::asio::io_context context;
    cpr::ThreadPool pool(context);
    auto canceled = pool.Submit([] { ADD_FAILURE() << "Canceled task executed"; });
    bool unrelatedRan{false};
    boost::asio::post(context, [&] { unrelatedRan = true; });
    pool.Stop();
    EXPECT_EQ(canceled.wait_for(0ms), std::future_status::ready);
    EXPECT_THROW(canceled.get(), std::future_error);
    EXPECT_FALSE(context.stopped());
    context.run();
    EXPECT_TRUE(unrelatedRan);
}

TEST(AsioContextTests, PendingHandlersAreSafeAfterPoolDestruction) {
    boost::asio::io_context context;
    std::future<int> canceled;
    {
        cpr::ThreadPool pool(context);
        canceled = pool.Submit([] { return 42; });
    }
    EXPECT_EQ(canceled.wait_for(0ms), std::future_status::ready);
    EXPECT_THROW(canceled.get(), std::future_error);
    EXPECT_NO_THROW(context.run());
}

TEST(AsioContextTests, RestartDoesNotDuplicateWorkFromOldHandlers) {
    boost::asio::io_context context;
    cpr::ThreadPool pool(context);
    auto canceled = pool.Submit([] {});
    pool.Stop();
    EXPECT_THROW(canceled.get(), std::future_error);
    size_t completed{0};
    auto queued = pool.Submit([&] { ++completed; });
    pool.Wait();
    pool.Start();
    context.run();
    queued.get();
    pool.Wait();
    EXPECT_EQ(completed, 1);
}

TEST(AsioContextTests, StopWaitsForActiveTasksWithoutStoppingContext) {
    boost::asio::io_context context;
    ContextRunner runner(context);
    cpr::ThreadPool pool(context, 0, 1);
    std::promise<void> started;
    std::promise<void> release;
    auto gate = release.get_future().share();
    auto active = pool.Submit([&] {
        started.set_value();
        gate.wait();
    });
    EXPECT_EQ(started.get_future().wait_for(5s), std::future_status::ready);
    auto canceled = pool.Submit([] {});
    auto stopping = std::async(std::launch::async, [&] { pool.Stop(); });
    EXPECT_EQ(canceled.wait_for(5s), std::future_status::ready);
    EXPECT_EQ(stopping.wait_for(0ms), std::future_status::timeout);
    release.set_value();
    stopping.get();
    EXPECT_NO_THROW(active.get());
    EXPECT_THROW(canceled.get(), std::future_error);
    EXPECT_FALSE(context.stopped());
    std::promise<void> unrelated;
    boost::asio::post(context, [&] { unrelated.set_value(); });
    EXPECT_EQ(unrelated.get_future().wait_for(5s), std::future_status::ready);
}

TEST(AsioContextTests, LimitsConcurrencyOnSharedContext) {
    boost::asio::io_context context;
    ContextRunner runner(context, 4);
    cpr::ThreadPool pool(context, 0, 2);
    std::atomic_size_t executing{0};
    std::atomic_size_t peak{0};
    std::atomic_size_t completed{0};
    for (size_t i = 0; i < 500; ++i) {
        pool.Submit([&] {
            auto count = ++executing;
            auto previous = peak.load();
            while (previous < count && !peak.compare_exchange_weak(previous, count)) {
            }
            std::this_thread::yield();
            --executing;
            ++completed;
        });
    }
    pool.Wait();
    EXPECT_EQ(completed, 500);
    EXPECT_LE(peak, 2);
    pool.SetThreadCounts(0, 1);
    EXPECT_EQ(pool.Submit([] { return 42; }).get(), 42);
    EXPECT_FALSE(context.stopped());
}

TEST(AsioContextTests, WorkerLifecycleCallsAreRejected) {
    boost::asio::io_context context;
    cpr::ThreadPool pool(context);
    auto result = pool.Submit([&] {
        EXPECT_THROW(pool.Stop(), std::logic_error);
        EXPECT_THROW(pool.Wait(), std::logic_error);
        EXPECT_THROW(pool.Start(), std::logic_error);
        EXPECT_THROW(pool.SetThreadCounts(0, 1), std::logic_error);
    });
    context.run();
    EXPECT_NO_THROW(result.get());
}

TEST(AsioContextTests, NestedContextDispatchRestoresWorkerIdentity) {
    boost::asio::io_context context;
    cpr::ThreadPool outer(context);
    cpr::ThreadPool inner(context);
    auto result = outer.Submit([&] {
        auto nested = inner.Submit([&] {
            EXPECT_THROW(outer.Wait(), std::logic_error);
            EXPECT_THROW(outer.Stop(), std::logic_error);
            EXPECT_THROW(outer.SetThreadCounts(0, 1), std::logic_error);
            return 42;
        });
        EXPECT_EQ(context.poll_one(), 1);
        EXPECT_EQ(nested.get(), 42);
        EXPECT_THROW(outer.Wait(), std::logic_error);
        EXPECT_THROW(outer.Stop(), std::logic_error);
    });
    context.run();
    EXPECT_NO_THROW(result.get());
}

TEST(GlobalAsioContextTests, FirstInitializationSelectsContextForAsyncRequests) {
    boost::asio::io_context rejected;
    EXPECT_THROW(cpr::async::startup(rejected, 0, 0), std::invalid_argument);
    boost::asio::io_context context;
    cpr::async::startup(context, 0, 1);
    auto* pool = cpr::GlobalThreadPool::GetInstance(context);
    EXPECT_EQ(cpr::GlobalThreadPool::GetInstance(), pool);
    boost::asio::io_context other;
    EXPECT_THROW(cpr::GlobalThreadPool::GetInstance(other), std::logic_error);
    auto result = cpr::async([] { return 42; });
    EXPECT_EQ(result.wait_for(0ms), std::future_status::timeout);
    context.run();
    EXPECT_EQ(result.get(), 42);
    context.restart();
    auto canceled = cpr::async([] {});
    bool unrelatedRan{false};
    boost::asio::post(context, [&] { unrelatedRan = true; });
    cpr::async::cleanup();
    EXPECT_THROW(canceled.get(), std::future_error);
    EXPECT_FALSE(context.stopped());
    context.run();
    EXPECT_TRUE(unrelatedRan);
    EXPECT_THROW(cpr::GlobalThreadPool::GetInstance(context), std::logic_error);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
