#include <gtest/gtest.h>

#include <string>

#include "cpr/cpr.h"

using namespace cpr;

TEST(EncodedAuthenticationTests, UnicodeEncoderTest) {
    std::string user = "一二三";
    std::string pass = "Hello World!";
    EncodedAuthentication pa{user, pass};
    EXPECT_EQ(pa.GetUsername(), "%E4%B8%80%E4%BA%8C%E4%B8%89");
    EXPECT_EQ(pa.GetPassword(), "Hello%20World%21");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
