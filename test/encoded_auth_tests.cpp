#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

using namespace cpr;

TEST(EncodedAuthenticationTests, UnicodeEncoderTest) {
    std::string user = "一二三";
    std::string pass = "Hello World!";
    EncodedAuthentication pa{user, pass};
    std::string expected = "%E4%B8%80%E4%BA%8C%E4%B8%89:Hello%20World%21";
    EXPECT_EQ(pa.GetAuthString(), expected);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
