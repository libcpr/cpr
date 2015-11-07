#include <gtest/gtest.h>

#include <string>

#include <cpr.h>

#include "server.h"

using namespace cpr;

static Server* server = new Server();
auto base = server->GetBaseUrl();
auto baseSSL = server->GetBaseUrlSSL();

TEST(ErrorTests, BasicSSLFailure) {
    auto url = Url{baseSSL + "/hello.html"};
    auto response = cpr::Get(url);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::SSL_CONNECT_ERROR, response.error.code);
}

//Not terribly sure how to test other SSL error codes

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
    auto url = Url{"http://railstars.com"}; //my own site, and notoriously slow to load. Need a better exemplar
    auto response = cpr::Get(url, cpr::Timeout{1});
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, ProxyFailure) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Get(url, cpr::Proxies{{"http", "http://bad_host/"}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::PROXY_RESOLUTION_FAILURE, response.error.code);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
