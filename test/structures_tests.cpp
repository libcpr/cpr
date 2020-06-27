#include <gtest/gtest.h>

#include <string>

#include <cpr/payload.h>

using namespace cpr;

TEST(PayloadTests, UseStringVariableTest) {
    std::string value1 = "hello";
    std::string key2 = "key2";
    Payload payload {{"key1", value1}, {key2, "world"}};

    std::string expected = "key1=hello&key2=world";
    EXPECT_EQ(payload.content, expected);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
