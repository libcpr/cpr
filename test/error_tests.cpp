#include <gtest/gtest.h>

#include <string>

#include <cpr.h>
#include <curl/curl.h>


#include "server.h"

using namespace cpr;

static Server* server = new Server();
auto base = server->GetBaseUrl();
auto baseSSL = server->GetBaseUrlSSL();

TEST(ErrorTests, BasicSSLFailure) {
    //If CURL has SSL enabled, we should expect SSL_CONECT_ERROR
    //  If not, we shoujld expect UNSUPPORTED_PROTOCOL


    auto url = Url{baseSSL + "/hello.html"};
    auto response = cpr::Get(url);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);

    //This is ugly, because it presume CURL under the hood. Might be nice to have something in `cpr::Session`
    //  That could tell us if SSL is supported.
    auto curl_version = curl_version_info(CURLVERSION_NOW);
    auto expected = ErrorCode::UNSUPPORTED_PROTOCOL;
    if(curl_version->features & CURL_VERSION_SSL) {
        expected = ErrorCode::SSL_CONNECT_ERROR;
    }
    EXPECT_EQ(expected, response.error.code);

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
