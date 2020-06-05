#include <gtest/gtest.h>

#include <chrono>
#include <string>

#include <cpr/cpr.h>
#include <curl/curl.h>

#include "httpServer.hpp"
#include "httpsServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(ErrorTests, UnsupportedProtocolFailure) {
    auto url = Url{"urk://wat.is.this"};
    auto response = cpr::Get(url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::UNSUPPORTED_PROTOCOL, response.error.code);
}

TEST(ErrorTests, InvalidURLFailure) {
    auto url = Url{"???"};
    auto response = cpr::Get(url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::INVALID_URL_FORMAT, response.error.code);
}

TEST(ErrorTests, TimeoutFailure) {
    auto url = Url{server->GetBaseUrl() + "/timeout.html"};
    auto response = cpr::Get(url, cpr::Timeout{1});
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, ChronoTimeoutFailure) {
    auto url = Url{server->GetBaseUrl() + "/timeout.html"};
    auto response = cpr::Get(url, cpr::Timeout{std::chrono::milliseconds{1}});
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, ConnectTimeoutFailure) {
    auto url = Url{"http://localhost:67"};
    auto response = cpr::Get(url, cpr::ConnectTimeout{1});
    EXPECT_EQ(0, response.status_code);
    // Sometimes a CONNECTION_FAILURE happens before the OPERATION_TIMEDOUT:
    EXPECT_TRUE(response.error.code == ErrorCode::OPERATION_TIMEDOUT ||
                response.error.code == ErrorCode::CONNECTION_FAILURE);
}

TEST(ErrorTests, ChronoConnectTimeoutFailure) {
    auto url = Url{"http://localhost:67"};
    auto response = cpr::Get(url, cpr::ConnectTimeout{std::chrono::milliseconds{1}});
    EXPECT_EQ(0, response.status_code);
    // Sometimes a CONNECTION_FAILURE happens before the OPERATION_TIMEDOUT:
    EXPECT_TRUE(response.error.code == ErrorCode::OPERATION_TIMEDOUT ||
                response.error.code == ErrorCode::CONNECTION_FAILURE);
}

TEST(ErrorTests, LowSpeedTimeFailure) {
    auto url = Url{server->GetBaseUrl() + "/low_speed.html"};
    auto response = cpr::Get(url, cpr::LowSpeed{1000, 1});
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, LowSpeedBytesFailure) {
    auto url = Url{server->GetBaseUrl() + "/low_speed_bytes.html"};
    auto response = cpr::Get(url, cpr::LowSpeed{1000, 1});
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, ProxyFailure) {
    auto url = Url{server->GetBaseUrl() + "/hello.html"};
    auto response = cpr::Get(url, cpr::Proxies{{"http", "http://bad_host/"}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::PROXY_RESOLUTION_FAILURE, response.error.code);
}

TEST(ErrorTests, BoolFalseTest) {
    Error error;
    EXPECT_FALSE(error);
}

TEST(ErrorTests, BoolTrueTest) {
    Error error;
    error.code = ErrorCode::UNSUPPORTED_PROTOCOL;
    EXPECT_TRUE(error);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
