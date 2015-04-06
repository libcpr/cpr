#include <gtest/gtest.h>

#include <string>

#include <cpr.h>

#include "server.h"


static Server* server = new Server();
auto base = server->GetBaseUrl();

TEST(RedirectTests, TemporaryDefaultRedirectTest) {
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

TEST(RedirectTests, NoTemporaryRedirectTest) {
    auto url = Url{base + "/temporary_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(false);
    auto response = session.Get();
    auto expected_text = std::string{"Found"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(302, response.status_code);
}

TEST(RedirectTests, PermanentDefaultRedirectTest) {
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

TEST(RedirectTests, NoPermanentRedirectTest) {
    auto url = Url{base + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(false);
    auto response = session.Get();
    auto expected_text = std::string{"Moved Permanently"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
}

TEST(MaxRedirectsTests, ZeroMaxRedirectsSuccessTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(0);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(MaxRedirectsTests, ZeroMaxRedirectsFailureTest) {
    auto url = Url{base + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(0);
    auto response = session.Get();
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
}

TEST(MaxRedirectsTests, OneMaxRedirectsSuccessTest) {
    auto url = Url{base + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(1);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{base + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(MaxRedirectsTests, OneMaxRedirectsFailureTest) {
    auto url = Url{base + "/two_redirects.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(1);
    auto response = session.Get();
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{base + "/permanent_redirect.html"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
}

TEST(MaxRedirectsTests, TwoMaxRedirectsSuccessTest) {
    auto url = Url{base + "/two_redirects.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(2);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{base + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
