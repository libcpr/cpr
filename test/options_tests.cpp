#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(OptionsTests, BaseUrlTest) {
    Url url{server->GetBaseUrl() + "/"};
    Response response = cpr::Options(url);
    std::string expected_text{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(OptionsTests, SpecificUrlTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Options(url);
    std::string expected_text{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(OptionsTests, AsyncBaseUrlTest) {
    Url url{server->GetBaseUrl() + "/"};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::OptionsAsync(url));
    }
    for (cpr::AsyncResponse& future_response : responses) {
        cpr::Response response = future_response.get();
        std::string expected_text{""};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(OptionsTests, AsyncSpecificUrlTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::OptionsAsync(url));
    }
    for (cpr::AsyncResponse& future_response : responses) {
        cpr::Response response = future_response.get();
        std::string expected_text{""};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
