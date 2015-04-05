#include <gtest/gtest.h>

#include <string>

#include <cpr.h>

#include "server.h"


static Server* server;
auto base = Url{"http://127.0.0.1:8080"};

TEST(SessionTests, TemporaryDefaultRedirectTest) {
    auto url = Url{base + "/temporary_redirect.html"};
    Session session;
    session.SetUrl(url);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{base + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(SessionTests, NoTemporaryRedirectTest) {
    auto url = Url{base + "/temporary_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(false);
    auto response = session.Get();
    auto expected_text = std::string{"Found"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{""}, response.header["content-type"]);
    EXPECT_EQ(302, response.status_code);
}

TEST(SessionTests, PermanentDefaultRedirectTest) {
    auto url = Url{base + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{base + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(SessionTests, NoPermanentRedirectTest) {
    auto url = Url{base + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(false);
    auto response = session.Get();
    auto expected_text = std::string{"Moved Permanently"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{""}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
}

int main(int argc, char** argv) {
    server = new Server();
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
