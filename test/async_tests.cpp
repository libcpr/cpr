#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(UrlEncodedPostTests, AsyncGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    cpr::AsyncResponse future = cpr::GetAsync(url);
    std::string expected_text{"Hello world!"};
    cpr::Response response = future.get();
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(UrlEncodedPostTests, AsyncGetMultipleTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::GetAsync(url));
    }
    for (auto& future : responses) {
        std::string expected_text{"Hello world!"};
        cpr::Response response = future.get();
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
}

TEST(UrlEncodedPostTests, AsyncGetMultipleReflectTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 100; ++i) {
        Parameters p{{"key", std::to_string(i)}};
        responses.emplace_back(cpr::GetAsync(url, p));
    }
    int i = 0;
    for (auto& future : responses) {
        std::string expected_text{"Hello world!"};
        cpr::Response response = future.get();
        EXPECT_EQ(expected_text, response.text);
        Url expected_url{url + "?key=" + std::to_string(i)};
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
