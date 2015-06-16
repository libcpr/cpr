#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include <cpr.h>

#include "cookies.h"
#include "multipart.h"
#include "server.h"
#include <iostream>
#include <list>


static Server* server = new Server();
auto base = server->GetBaseUrl();


TEST(UrlEncodedPostTests, AsyncGetTest) {
    auto url = Url{base + "/hello.html"};
    auto future = cpr::GetAsync(url);
    auto expected_text = std::string{"Hello world!"};
    auto response = future.get();
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(UrlEncodedPostTests, AsyncGetMultipleTest) {
    auto url = Url{base + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::GetAsync(url));
    }
    for (auto& future : responses) {
        auto expected_text = std::string{"Hello world!"};
        auto response = future.get();
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
}

TEST(UrlEncodedPostTests, AsyncGetMultipleReflectTest) {
    auto url = Url{base + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 100; ++i) {
        auto p = Parameters{{"key", std::to_string(i)}};
        responses.emplace_back(cpr::GetAsync(url, p));
    }
    int i = 0;
    for (auto& future : responses) {
        auto expected_text = std::string{"Hello world!"};
        auto response = future.get();
        EXPECT_EQ(expected_text, response.text);
        auto expected_url = Url{url + "?key=" + std::to_string(i)};
        EXPECT_EQ(expected_url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        ++i;
    }
}

TEST(CookiesTests, CookiesTest) {
    auto url = Url{base + "/basic_cookies.html"};
    Session session{};
    session.SetUrl(url);
    Cookies cookies;

    {
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        cookies = response.cookies;
    }
    {
        cookies["hello"] = "world";
        cookies["my"] = "another; fake=cookie;"; // This is url encoded
        session.SetCookies(cookies);
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(cookies["cookie"], response.cookies["cookie"]);
        EXPECT_EQ(cookies["icecream"], response.cookies["icecream"]);
        EXPECT_EQ(cookies["expires"], response.cookies["expires"]);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
