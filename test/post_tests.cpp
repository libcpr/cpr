#include <gtest/gtest.h>

#include <string>

#include <cpr.h>

#include "server.h"


static Server* server = new Server();
auto base = server->GetBaseUrl();

TEST(PostTests, PostSingleTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Post(url, Payload{{"hello", "world"}});
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(PostTests, PostManyTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Post(url, Payload{{"hello", "world"}, {"more", "values"}});
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(PostTests, PostBadHostTest) {
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
