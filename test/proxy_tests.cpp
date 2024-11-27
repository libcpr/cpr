#include <gtest/gtest.h>

#include <chrono>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "cpr/cpr.h"

// TODO: This uses public servers for proxies and endpoints. This should be replaced with a source
// code implementation inside server.cpp

// NOTES:
// * For no-proxy testing need to run the tests with direct connection to the internet
// * List of free proxies for testing can be found at https://proxy-list.org/english/index.php
//   Example: #define HTTP_PROXY "http://162.223.90.130:80"
#define HTTP_PROXY "51.159.4.98:80"
#define HTTPS_PROXY "51.104.53.182:8000"


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
    session.SetTimeout(std::chrono::seconds(10));
    Response response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ProxyTests, NoProxyTest) {
    setenv("NO_PROXY", "httpbin.org", 1);
    try {
        Url url{"http://www.httpbin.org/get"};
        Proxies proxies{{"http", HTTP_PROXY}, {"no_proxy", ""}};
        Session session;
        session.SetUrl(url);
        session.SetProxies(proxies);
        session.SetTimeout(std::chrono::seconds(10));
        Response response = session.Get();
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);

        // check that access was performed through the proxy
        std::string proxy_ip = HTTP_PROXY;
        if (proxy_ip[0] == 'h') {
            // drop protocol:
            proxy_ip = proxy_ip.substr(proxy_ip.find(':') + 3);
        }
        // drop port:
        proxy_ip = proxy_ip.substr(0, proxy_ip.find(':'));

        // find "origin": "ip" in response:
        bool found = false;
        std::istringstream body(response.text);
        std::string line;
        while (std::getline(body, line)) {
            // example: "origin": "123.456.789.123"
            if (line.find("\"origin\":") != std::string::npos) {
                found = line.find(proxy_ip) != std::string::npos;
                break;
            }
        }
        EXPECT_TRUE(found);
    } catch (...) {
        unsetenv("NO_PROXY");
        throw;
    }
    unsetenv("NO_PROXY");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
