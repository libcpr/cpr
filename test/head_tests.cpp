#include <chrono>
#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(HeadTests, BasicHeadTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, ComplexHeadTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    Response response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, ResourceNotFoundHeadTest) {
    Url url{server->GetBaseUrl() + "/error.html"};
    Response response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(404, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, BadHostHeadTest) {
    Url url{"http://bad_host/"};
    Response response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::HOST_RESOLUTION_FAILURE, response.error.code);
}

TEST(HeadTests, CookieHeadTest) {
    Url url{server->GetBaseUrl() + "/basic_cookies.html"};
    Response response = cpr::Head(url);
    cpr::Cookies expectedCookies{
            {"SID", "31d4d96e407aad42", "127.0.0.1", false, "/", true, std::chrono::system_clock::from_time_t(3905119080)},
            {"lang", "en-US", "127.0.0.1", false, "/", true, std::chrono::system_clock::from_time_t(3905119080)},
    };
    cpr::Cookies res_cookies{response.cookies};
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
    for (auto cookie = res_cookies.begin(), expectedCookie = expectedCookies.begin(); cookie != res_cookies.end() && expectedCookie != expectedCookies.end(); cookie++, expectedCookie++) {
        EXPECT_EQ(expectedCookie->GetName(), cookie->GetName());
        EXPECT_EQ(expectedCookie->GetValue(), cookie->GetValue());
        EXPECT_EQ(expectedCookie->GetDomain(), cookie->GetDomain());
        EXPECT_EQ(expectedCookie->IsIncludingSubdomains(), cookie->IsIncludingSubdomains());
        EXPECT_EQ(expectedCookie->GetPath(), cookie->GetPath());
        EXPECT_EQ(expectedCookie->IsHttpsOnly(), cookie->IsHttpsOnly());
        EXPECT_EQ(expectedCookie->GetExpires(), cookie->GetExpires());
    }
}

TEST(HeadTests, ParameterHeadTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Parameters parameters{{"key", "value"}};
    Response response = cpr::Head(url, parameters);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?key=value"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, AuthenticationSuccessHeadTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Head(url, Authentication{"user", "password", AuthMode::BASIC});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, AuthenticationNullFailureHeadTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, AuthenticationFailureHeadTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Head(url, Authentication{"user", "bad_password", AuthMode::BASIC});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, BearerSuccessHeadTest) {
    Url url{server->GetBaseUrl() + "/bearer_token.html"};
#if CPR_LIBCURL_VERSION_NUM >= 0x073D00
    Response response = cpr::Get(url, Bearer{"the_token"});
#else
    Response response = cpr::Get(url, Header{{"Authorization", "Bearer the_token"}});
#endif
    EXPECT_EQ(std::string{"Header reflect GET"}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, DigestSuccessHeadTest) {
    Url url{server->GetBaseUrl() + "/digest_auth.html"};
    Response response = cpr::Head(url, Authentication{"user", "password", AuthMode::DIGEST});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, HeaderReflectNoneHeadTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Head(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, HeaderReflectEmptyHeadTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Head(url, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, HeaderReflectHeadTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Head(url, Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, SetEmptyHeaderHeadTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Head(url, Header{{"hello", ""}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, RedirectHeadTest) {
    Url url{server->GetBaseUrl() + "/temporary_redirect.html"};
    Response response = cpr::Head(url, Redirect(false));
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(302, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, ZeroMaxRedirectsHeadTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Head(url, Redirect(0L));
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, BasicHeadAsyncTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::HeadAsync(url));
    }
    for (cpr::AsyncResponse& future_response : responses) {
        cpr::Response response = future_response.get();
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
