#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(OptionsTests, BaseUrlTest) {
    auto url = Url{server->GetBaseUrl() + "/"};
    auto response = cpr::Options(url);
    auto expected_text = std::string{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"},
              response.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(OptionsTests, SpecificUrlTest) {
    auto url = Url{server->GetBaseUrl() + "/hello.html"};
    auto response = cpr::Options(url);
    auto expected_text = std::string{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"GET, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(OptionsTests, AsyncBaseUrlTest) {
    auto url = Url{server->GetBaseUrl() + "/"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::OptionsAsync(url));
    }
    for (auto& future_response : responses) {
        auto response = future_response.get();
        auto expected_text = std::string{""};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"},
                  response.header["Access-Control-Allow-Methods"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(OptionsTests, AsyncSpecificUrlTest) {
    auto url = Url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::OptionsAsync(url));
    }
    for (auto& future_response : responses) {
        auto response = future_response.get();
        auto expected_text = std::string{""};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"GET, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
