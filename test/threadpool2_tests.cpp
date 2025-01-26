#include <atomic>
#include <cstddef>
#include <gtest/gtest.h>

#include "cpr/threadpool2.h"

TEST(ThreadPool2Tests, BasicWorkOneThread) {
    std::atomic_uint32_t invCount{0};
    uint32_t invCountExpected{100};

    {
        cpr::ThreadPool2 tp(1, 1);

        for (size_t i = 0; i < invCountExpected; ++i) {
            tp.Submit([&invCount]() -> void { invCount++; });
        }

        // Wait for the thread pool to finish its work
        tp.Wait();
    }

    EXPECT_EQ(invCount, invCountExpected);
}

TEST(ThreadPool2Tests, BasicWorkMultipleThreads) {
    std::atomic_uint32_t invCount{0};
    uint32_t invCountExpected{100};

    {
        cpr::ThreadPool2 tp(1, 10);

        for (size_t i = 0; i < invCountExpected; ++i) {
            tp.Submit([&invCount]() -> void { invCount++; });
        }

        // Wait for the thread pool to finish its work
        tp.Wait();
    }

    EXPECT_EQ(invCount, invCountExpected);
}

// Ensure only the current task gets finished when stopping worker
TEST(ThreadPool2Tests, CanceledBeforeDone) {
    std::atomic_uint32_t invCount{0};
    {
        cpr::ThreadPool2 tp(1, 1);

        for (size_t i = 0; i < 100; ++i) {
            tp.Submit([&invCount]() -> void {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                invCount++;
            });
        }
    }

    EXPECT_EQ(invCount, 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
