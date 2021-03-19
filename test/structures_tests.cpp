#include "cpr/cprtypes.h"
#include <gtest/gtest.h>

#include <string>

#include <cpr/payload.h>
#include <cpr/parameters.h>

using namespace cpr;

TEST(PayloadTests, UseStringVariableTest) {
    std::string value1 = "hello";
    std::string key2 = "key2";
    Payload payload {{"key1", value1}, {key2, "world"}};

    std::string expected = "key1=hello&key2=world";
    EXPECT_EQ(payload.GetContent(CurlHolder()), expected);
}

TEST(PayloadTests, DisableEncodingTest) {
    std::string key1 = "key1";
    std::string key2 = "key2§$%&/";
    std::string value1 = "hello.,.,";
    std::string value2 = "hello";
    Payload payload{{key1, value1}, {key2, value2}};
    payload.encode = false;

    std::string expected = key1 + '=' + value1 + '&' + key2 + '=' + value2;
    EXPECT_EQ(payload.GetContent(CurlHolder()), expected);
}

TEST(ParametersTests, UseStringVariableTest) {
    std::string value1 = "hello";
    std::string key2 = "key2";
    Parameters parameters {{"key1", value1}, {key2, "world"}};

    std::string expected = "key1=hello&key2=world";
    EXPECT_EQ(parameters.GetContent(CurlHolder()), expected);
}

TEST(ParametersTests, DisableEncodingTest) {
    std::string key1 = "key1";
    std::string key2 = "key2§$%&/";
    std::string value1 = "hello.,.,";
    std::string value2 = "hello";
    Parameters parameters{{key1, value1}, {key2, value2}};
    parameters.encode = false;

    std::string expected = key1 + '=' + value1 + '&' + key2 + '=' + value2;
    EXPECT_EQ(parameters.GetContent(CurlHolder()), expected);
}

TEST(UrlToAndFromString, UrlTests) {
    std::string s{"https://github.com/whoshuu/cpr"};
    cpr::Url url = s;
    EXPECT_EQ(s, url.str());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
