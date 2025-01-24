#include <atomic>
#include <cstddef>
#include <gtest/gtest.h>

#include "cpr/threadpool2.h"

TEST(ThreadPool2Tests, StartStop) {
    cpr::ThreadPool2 tp(1, 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
