#include <gtest/gtest.h>

#include <string>

#include <cpr.h>


// TODO: This uses public servers for proxies and endpoints. This should be replaced with a source
// code implementation inside server.cpp

TEST(ProxyTests, SingleProxyTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    auto response = cpr::Get(url, Proxies{{"http", "173.234.8.2:80"}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(ProxyTests, MultipleProxyHttpTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    auto response = cpr::Get(url, Proxies{{"http", "173.234.8.2:80"},
                                          {"https", "67.195.42.72:80"}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(ProxyTests, MultipleProxyHttpsTest) {
    auto url = Url{"https://www.httpbin.org/get"};
    auto response = cpr::Get(url, Proxies{{"http", "173.234.8.2:80"},
                                          {"https", "67.195.42.72:80"}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(ProxyTests, ProxySessionTest) {
    auto url = Url{"http://www.httpbin.org/get"};
    Session session;
    session.SetUrl(url);
    session.SetProxies(Proxies{{"http", "173.234.8.2:80"}});
    auto response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
