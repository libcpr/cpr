#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>

#include "server.h"


static Server* server = new Server();
auto base = server->GetBaseUrl();


TEST(UrlEncodedPostTests, AsyncGetTest) {
    auto url = Url{base + "/hello.html"};
    auto future = cpr::GetAsync(url);
    auto expected_text = std::string{"Hello world!"};
    auto response = future.get();
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(UrlEncodedPostTests, AsyncGetMultipleTest) {
    auto url = Url{base + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::GetAsync(url));
    }
    for (auto& future : responses) {
        auto expected_text = std::string{"Hello world!"};
        auto response = future.get();
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
}

TEST(UrlEncodedPostTests, AsyncGetMultipleReflectTest) {
    auto url = Url{base + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 100; ++i) {
        auto p = Parameters{{"key", std::to_string(i)}};
        responses.emplace_back(cpr::GetAsync(url, p));
    }
    int i = 0;
    for (auto& future : responses) {
        auto expected_text = std::string{"Hello world!"};
        auto response = future.get();
        EXPECT_EQ(expected_text, response.text);
        auto expected_url = Url{url + "?key=" + std::to_string(i)};
        EXPECT_EQ(expected_url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        ++i;
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
