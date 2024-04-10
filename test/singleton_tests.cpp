#include <gtest/gtest.h>

#include "singleton_tests.hpp"

// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
CPR_SINGLETON_IMPL(TestSingleton)

TEST(SingletonTests, GetInstanceTest) {
    const TestSingleton* singleton = TestSingleton::GetInstance();
    EXPECT_NE(singleton, nullptr);
}

TEST(SingletonTests, ExitInstanceTest) {
    TestSingleton* singleton = TestSingleton::GetInstance();
    TestSingleton::ExitInstance();
    singleton = TestSingleton::GetInstance();
    EXPECT_EQ(singleton, nullptr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
