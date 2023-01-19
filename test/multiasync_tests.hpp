#pragma once

#include <atomic>
#include <condition_variable>
#include <gtest/gtest.h>

class TestSynchronizationEnv : public testing::Environment {
  public:
    std::atomic_size_t counter{0};
    std::atomic_bool fn_called{false};
    std::condition_variable test_cv{};
    std::mutex test_cv_mutex{};

    void Reset() {
        counter = 0;
        fn_called = false;
    }
};
