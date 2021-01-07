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
    Cookies cookies{{"hello", "world"}, {"my", "another; fake=cookie;"}};
    Response response = cpr::Head(url, cookies);
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
    Response response = cpr::Head(url, Authentication{"user", "password"});
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
    Response response = cpr::Head(url, Authentication{"user", "bad_password"});
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
    Response response = cpr::Head(url, Digest{"user", "password"});
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
    Response response = cpr::Head(url, false);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(302, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeadTests, ZeroMaxRedirectsHeadTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Head(url, 0L);
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
