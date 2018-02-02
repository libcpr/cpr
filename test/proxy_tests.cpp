#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

// TODO: This uses public servers for proxies and endpoints. This should be replaced with a source
// code implementation inside server.cpp

#define HTTP_PROXY "104.131.214.38:3128"
#define HTTPS_PROXY "104.131.214.38:3128"

using namespace cpr;

TEST(ProxyTests, SingleProxyTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    auto response = cpr::Get(url, Proxies{{"http", HTTP_PROXY}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, MultipleProxyHttpTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    auto response = cpr::Get(url, Proxies{{"http", HTTP_PROXY},
                                          {"https", HTTPS_PROXY}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

// TODO: These should be fixed after a source code implementation of an HTTPS proxy
#if defined(false)
TEST(ProxyTests, ProxyHttpsTest) {
    auto url = Url{"https://www.httpbin.org/get"};
    auto response = cpr::Get(url, Proxies{{"https", HTTPS_PROXY}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, MultipleProxyHttpsTest) {
    auto url = Url{"https://www.httpbin.org/get"};
    auto response = cpr::Get(url, Proxies{{"http", HTTP_PROXY},
                                          {"https", HTTPS_PROXY}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}
#endif

TEST(ProxyTests, CopyProxyTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    auto proxies = Proxies{{"http", HTTP_PROXY}};
    auto response = cpr::Get(url, proxies);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, ProxySessionTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    Session session;
    session.SetUrl(url);
    session.SetProxies(Proxies{{"http", HTTP_PROXY}});
    auto response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, ReferenceProxySessionTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    auto proxies = Proxies{{"http", HTTP_PROXY}};
    Session session;
    session.SetUrl(url);
    session.SetProxies(proxies);
    auto response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

// TODO: implement a server proxy that we could use locally that uses username and password
#if defined(false)
TEST(ProxyTests, ProxyAuthHttpTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    auto response = cpr::Get(url,
                             Proxies{{"http", "127.0.0.1:8080"}},
                             ProxyAuthentication{{"http", Authentication{"foo", "bar"}}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, ProxyAuthHttpsTest) {
    auto url = Url{"https://www.httpbin.org/get"};
    auto response = cpr::Get(url,
                             Proxies{{"https", "127.0.0.1:8080"}},
                             ProxyAuthentication{{"https", Authentication{"foo", "bar"}}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, ProxyAuthMultipleProxyHttpTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    auto response = cpr::Get(url,
                             Proxies{{"http", "127.0.0.1:8080"},
                                     {"https", "127.0.0.1:8080"}},
                             ProxyAuthentication{{"http", Authentication{"foo", "bar"}},
                                                 {"https", Authentication{"foo", "bar"}}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, ProxyAuthMultipleProxyHttpsTest) {
    auto url = Url{"https://www.httpbin.org/get"};
    auto response = cpr::Get(url,
                             Proxies{{"http", "127.0.0.1:8080"},
                                     {"https", "127.0.0.1:8080"}},
                             ProxyAuthentication{{"http", Authentication{"foo", "bar"}},
                                                 {"https", Authentication{"foo", "bar"}}});
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
