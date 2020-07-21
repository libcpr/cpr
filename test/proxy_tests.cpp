#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

// TODO: This uses public servers for proxies and endpoints. This should be replaced with a source
// code implementation inside server.cpp

#define HTTP_PROXY "104.131.214.38:3128"
#define HTTPS_PROXY "104.131.214.38:3128"

using namespace cpr;

TEST(ProxyTests, SingleProxyTest) {
    Url url{"http://www.httpbin.org/get"};
    Response response = cpr::Get(url, Proxies{{"http", HTTP_PROXY}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, MultipleProxyHttpTest) {
    Url url{"http://www.httpbin.org/get"};
    Response response = cpr::Get(url, Proxies{{"http", HTTP_PROXY}, {"https", HTTPS_PROXY}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

// TODO: These should be fixed after a source code implementation of an HTTPS proxy
#if defined(false)
TEST(ProxyTests, ProxyHttpsTest) {
    Url url{"https://www.httpbin.org/get"};
    Response response = cpr::Get(url, Proxies{{"https", HTTPS_PROXY}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, MultipleProxyHttpsTest) {
    Url url{"https://www.httpbin.org/get"};
    Response response = cpr::Get(url, Proxies{{"http", HTTP_PROXY}, {"https", HTTPS_PROXY}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}
#endif

TEST(ProxyTests, CopyProxyTest) {
    Url url{"http://www.httpbin.org/get"};
    Proxies proxies{{"http", HTTP_PROXY}};
    Response response = cpr::Get(url, proxies);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, ProxySessionTest) {
    Url url{"http://www.httpbin.org/get"};
    Session session;
    session.SetUrl(url);
    session.SetProxies(Proxies{{"http", HTTP_PROXY}});
    Response response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, ReferenceProxySessionTest) {
    Url url{"http://www.httpbin.org/get"};
    Proxies proxies{{"http", HTTP_PROXY}};
    Session session;
    session.SetUrl(url);
    session.SetProxies(proxies);
    Response response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
