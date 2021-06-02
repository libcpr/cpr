#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

// TODO: This uses private proxy servers and public servers for endpoints. This should be replaced with a source
// code implementation inside server.cpp

#define HTTP_PROXY "192.168.0.180:3128"
#define HTTPS_PROXY "192.168.0.180:3128"
#define PROXY_USER "u$er"
#define PROXY_PASS "p@ss"

using namespace cpr;

// TODO: These should be fixed after a source code implementation of a proxy
#if defined(false)
TEST(ProxyAuthTests, SingleProxyTest) {
    Url url{"https://www.httpbin.org/get"};
    Response response = cpr::Get(url, Proxies{{"https", HTTP_PROXY}}, ProxyAuthentication{{"https", EncodedAuthentication{PROXY_USER, PROXY_PASS}}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, MultipleProxyHttpTest) {
    Url url{"http://www.httpbin.org/get"};
    Response response = cpr::Get(url, Proxies{{"http", HTTP_PROXY}, {"https", HTTPS_PROXY}}, ProxyAuthentication{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}, {"https", EncodedAuthentication{PROXY_USER, PROXY_PASS}}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, ProxyHttpsTest) {
    Url url{"https://www.httpbin.org/get"};
    Response response = cpr::Get(url, Proxies{{"https", HTTPS_PROXY}}, ProxyAuthentication{{"https", EncodedAuthentication{PROXY_USER, PROXY_PASS}}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, MultipleProxyHttpsTest) {
    Url url{"https://www.httpbin.org/get"};
    Response response = cpr::Get(url, Proxies{{"http", HTTP_PROXY}, {"https", HTTPS_PROXY}}, ProxyAuthentication{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}, {"https", EncodedAuthentication{PROXY_USER, PROXY_PASS}}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, CopyProxyTest) {
    Url url{"http://www.httpbin.org/get"};
    Proxies proxies{{"http", HTTP_PROXY}};
    ProxyAuthentication proxy_auth{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}};
    Response response = cpr::Get(url, proxies, proxy_auth);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, ProxySessionTest) {
    Url url{"http://www.httpbin.org/get"};
    Session session;
    session.SetUrl(url);
    session.SetProxies(Proxies{{"http", HTTP_PROXY}});
    session.SetProxyAuth(ProxyAuthentication{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}});
    Response response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyAuthTests, ReferenceProxySessionTest) {
    Url url{"http://www.httpbin.org/get"};
    Proxies proxies{{"http", HTTP_PROXY}};
    ProxyAuthentication proxy_auth{{"http", EncodedAuthentication{PROXY_USER, PROXY_PASS}}};
    Session session;
    session.SetUrl(url);
    session.SetProxies(proxies);
    session.SetProxyAuth(proxy_auth);
    Response response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
