#include <gtest/gtest.h>

#include <string>

#include <cpr.h>

#include "server.h"


static Server* server = new Server();
auto base = server->GetBaseUrl();

TEST(UrlEncodedPostTests, UrlPostSingleTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Payload{{"x", "5"}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
}

TEST(UrlEncodedPostTests, UrlPostManyTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Payload{{"x", "5"}, {"y", "13"}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5,\n"
                                     "  \"y\": 13,\n"
                                     "  \"sum\": 18\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
}

TEST(UrlEncodedPostTests, UrlPostBadHostTest) {
    auto url = Url{"http://bad_host/"};
    auto response = cpr::Post(url, Payload{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(0, response.status_code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
