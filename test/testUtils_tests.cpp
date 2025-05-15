#include <cstdint>
#include <gtest/gtest.h>

#include "testUtils.hpp"

TEST(TestUtils, GetUnusedPort) {
    uint16_t port{0};
    ASSERT_NO_THROW(port = cpr::test::get_free_port());
    EXPECT_NE(port, 0);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
