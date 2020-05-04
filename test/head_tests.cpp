#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(HeadTests, BasicHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/hello.html"};
    auto response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, ComplexHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/basic.json"};
    auto response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, ResourceNotFoundHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/error.html"};
    auto response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(404, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, BadHostHeadTest) {
    auto url = Url{"http://bad_host/"};
    auto response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::HOST_RESOLUTION_FAILURE, response.error.code);
}

TEST(HeadTests, CookieHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/basic_cookies.html"};
    auto cookies = Cookies{{"hello", "world"}, {"my", "another; fake=cookie;"}};
    auto response = cpr::Head(url, cookies);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
    cookies = response.cookies;
    EXPECT_EQ(cookies["cookie"], response.cookies["cookie"]);
    EXPECT_EQ(cookies["icecream"], response.cookies["icecream"]);
    EXPECT_EQ(cookies["expires"], response.cookies["expires"]);
}

TEST(HeadTests, ParameterHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/hello.html"};
    auto parameters = Parameters{{"key", "value"}};
    auto response = cpr::Head(url, parameters);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?key=value"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, AuthenticationSuccessHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/basic_auth.html"};
    auto response = cpr::Head(url, Authentication{"user", "password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, AuthenticationNullFailureHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/basic_auth.html"};
    auto response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, AuthenticationFailureHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/basic_auth.html"};
    auto response = cpr::Head(url, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, DISABLED_DigestSuccessHeadTest) { // Is nondeterministic using embedded mongoose
    auto url = Url{server->GetBaseUrl() + "/digest_auth.html"};
    auto response = cpr::Head(url, Digest{"user", "password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, HeaderReflectNoneHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/header_reflect.html"};
    auto response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, HeaderReflectEmptyHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/header_reflect.html"};
    auto response = cpr::Head(url, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, HeaderReflectHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/header_reflect.html"};
    auto response = cpr::Head(url, Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, SetEmptyHeaderHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/header_reflect.html"};
    auto response = cpr::Head(url, Header{{"hello", ""}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, RedirectHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/temporary_redirect.html"};
    auto response = cpr::Head(url, false);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(302, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, ZeroMaxRedirectsHeadTest) {
    auto url = Url{server->GetBaseUrl() + "/hello.html"};
    auto response = cpr::Head(url, 0L);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, BasicHeadAsyncTest) {
    auto url = Url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::HeadAsync(url));
    }
    for (auto& future_response : responses) {
        auto response = future_response.get();
        EXPECT_EQ(std::string{}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
