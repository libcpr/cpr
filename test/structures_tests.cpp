#include <gtest/gtest.h>

#include <string>

#include <cpr/payload.h>

using namespace cpr;

TEST(PayloadTests, AddPairTest) {
    Payload payload {{"key1", "hello"}};
    payload.AddPair({"key2", "world"});

    std::string expected = "key1=hello&key2=world";
    EXPECT_EQ(payload.content, expected);
}

TEST(PayloadTests, UseStringVariableTest) {
    std::string value1 = "hello";
    std::string key2 = "key2";
    Payload payload {{"key1", value1}, {key2, "world"}};

    std::string expected = "key1=hello&key2=world";
    EXPECT_EQ(payload.content, expected);
}

TEST(PayloadTests, Int64Test) {
    uint64_t a = 10000000000;
    int64_t  b= -10000000000;

    Payload payload {{"64bit", a}, {"neg64bit", b}, {"char", 'a'}};

    std::string expected = "64bit=10000000000&neg64bit=-10000000000&char=97";
    EXPECT_EQ(payload.content, expected);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
