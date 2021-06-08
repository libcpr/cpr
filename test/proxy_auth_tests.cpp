#include <gtest/gtest.h>

#include <string>

#include "cpr/cpr.h"
#include "httpServer.hpp"

// TODO: This requires a local proxy server (squid). This should be replaced with a source
// code implementation.

#define HTTP_PROXY "127.0.0.1:3128"
#define HTTPS_PROXY "127.0.0.1:3128"
#define PROXY_USER "u$er"
#define PROXY_PASS "p@ss"

using namespace cpr;

static HttpServer* server = new HttpServer();

// TODO: These should be fixed after a source code implementation of a proxy
#if defined(false)
TEST(ProxyAuthTests, SingleProxyTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Get(url, Proxies{{"http", HTTP_PROXY}}, ProxyAuthentication{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}});
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, MultipleProxyHttpTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Get(url, Proxies{{"https", HTTPS_PROXY}, {"http", HTTP_PROXY}}, ProxyAuthentication{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}, {"https", EncodedAuthentication{PROXY_USER, PROXY_PASS}}});
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, CopyProxyTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Proxies proxies{{"http", HTTP_PROXY}};
    ProxyAuthentication proxy_auth{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}};
    Response response = cpr::Get(url, proxies, proxy_auth);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, ProxySessionTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetProxies(Proxies{{"http", HTTP_PROXY}});
    session.SetProxyAuth(ProxyAuthentication{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}});
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, ReferenceProxySessionTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Proxies proxies{{"http", HTTP_PROXY}};
    ProxyAuthentication proxy_auth{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}};
    Session session;
    session.SetUrl(url);
    session.SetProxies(proxies);
    session.SetProxyAuth(proxy_auth);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
