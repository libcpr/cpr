#include <gtest/gtest.h>

#include <chrono>
#include <string>

#include <cpr/cpr.h>
#include <curl/curl.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();
std::chrono::milliseconds sleep_time{50};
std::chrono::seconds zero{0};

bool write_data(std::string /*data*/, intptr_t /*userdata*/) {
    return true;
}

TEST(RedirectTests, TemporaryDefaultRedirectTest) {
    Url url{server->GetBaseUrl() + "/temporary_redirect.html"};
    Session session;
    session.SetUrl(url);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RedirectTests, NoTemporaryRedirectTest) {
    Url url{server->GetBaseUrl() + "/temporary_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(Redirect(false));
    Response response = session.Get();
    std::string expected_text{"Moved Temporarily"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(302, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RedirectTests, PermanentDefaultRedirectTest) {
    Url url{server->GetBaseUrl() + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RedirectTests, NoPermanentRedirectTest) {
    Url url{server->GetBaseUrl() + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(Redirect(false));
    Response response = session.Get();
    std::string expected_text{"Moved Permanently"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, ZeroMaxRedirectsSuccessTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(Redirect(0L));
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, ZeroMaxRedirectsFailureTest) {
    Url url{server->GetBaseUrl() + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(Redirect(0L));
    Response response = session.Get();
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
    EXPECT_EQ(ErrorCode::TOO_MANY_REDIRECTS, response.error.code);
}

TEST(MaxRedirectsTests, OneMaxRedirectsSuccessTest) {
    Url url{server->GetBaseUrl() + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(Redirect(1L));
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, OneMaxRedirectsFailureTest) {
    Url url{server->GetBaseUrl() + "/two_redirects.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(Redirect(1L));
    Response response = session.Get();
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/permanent_redirect.html"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
    EXPECT_EQ(ErrorCode::TOO_MANY_REDIRECTS, response.error.code);
}

TEST(MaxRedirectsTests, TwoMaxRedirectsSuccessTest) {
    Url url{server->GetBaseUrl() + "/two_redirects.html"};
    Session session;
    session.SetUrl(url);
    session.SetRedirect(Redirect(2L));
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MultipleGetTests, BasicMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    for (size_t i = 0; i < 100; ++i) {
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, UrlChangeMultipleGetTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/hello.html"};
        session.SetUrl(url);
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        Url url{server->GetBaseUrl() + "/basic.json"};
        session.SetUrl(url);
        Response response = session.Get();
        std::string expected_text{
                "[\n"
                "  {\n"
                "    \"first_key\": \"first_value\",\n"
                "    \"second_key\": \"second_value\"\n"
                "  }\n"
                "]"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, HeaderMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    session.SetHeader(Header{{"hello", "world"}});
    for (size_t i = 0; i < 100; ++i) {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(std::string{"world"}, response.header["hello"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, HeaderChangeMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    session.SetHeader(Header{{"hello", "world"}});
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(std::string{"world"}, response.header["hello"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetHeader(Header{{"key", "value"}});
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(std::string{"value"}, response.header["key"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, ParameterMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetParameters({{"hello", "world"}});
    for (size_t i = 0; i < 100; ++i) {
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(Url{url + "?hello=world"}, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, ParameterChangeMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetParameters({{"hello", "world"}});
    {
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(Url{url + "?hello=world"}, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetUrl(url);
    session.SetParameters({{"key", "value"}});
    {
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(Url{url + "?key=value"}, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, BasicAuthenticationMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Session session;
    session.SetUrl(url);
    session.SetAuth(Authentication{"user", "password", AuthMode::BASIC});
    for (size_t i = 0; i < 100; ++i) {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, BasicAuthenticationChangeMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Session session;
    session.SetUrl(url);
    session.SetAuth(Authentication{"user", "password", AuthMode::BASIC});
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetAuth(Authentication{"user", "bad_password", AuthMode::BASIC});
    {
        Response response = session.Get();
        EXPECT_EQ(std::string{"Unauthorized"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
        EXPECT_EQ(401, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetAuth(Authentication{"bad_user", "password", AuthMode::BASIC});
    {
        Response response = session.Get();
        EXPECT_EQ(std::string{"Unauthorized"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
        EXPECT_EQ(401, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(ParameterTests, ParameterSingleTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    Parameters parameters{{"hello", "world"}};
    session.SetParameters(parameters);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterTests, ParameterMultipleTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    Parameters parameters{{"hello", "world"}, {"key", "value"}};
    session.SetParameters(parameters);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?hello=world&key=value"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(FullRequestUrlTest, GetFullRequestUrlNoParametersTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    std::string expected_text{server->GetBaseUrl() + "/hello.html"};
    EXPECT_EQ(expected_text, session.GetFullRequestUrl());
}

TEST(FullRequestUrlTest, GetFullRequestUrlOneParameterTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    Parameters parameters{{"hello", "world"}};
    session.SetParameters(parameters);
    std::string expected_text{server->GetBaseUrl() + "/hello.html" + "?hello=world"};
    EXPECT_EQ(expected_text, session.GetFullRequestUrl());
}

TEST(FullRequestUrlTest, GetFullRequestUrlMultipleParametersTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    Parameters parameters{{"hello", "world"}, {"key", "value"}};
    session.SetParameters(parameters);
    std::string expected_text{server->GetBaseUrl() + "/hello.html" + "?hello=world&key=value"};
    EXPECT_EQ(expected_text, session.GetFullRequestUrl());
}

TEST(TimeoutTests, SetTimeoutTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(0L);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(TimeoutTests, SetTimeoutLongTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(10000L);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(TimeoutTests, SetTimeoutLowSpeed) {
    Url url{server->GetBaseUrl() + "/low_speed_timeout.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(1000);
    Response response = session.Get();
    EXPECT_EQ(url, response.url);
    EXPECT_FALSE(response.status_code == 200);
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(TimeoutTests, SetChronoTimeoutTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(std::chrono::milliseconds{0});
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(TimeoutTests, SetChronoTimeoutLongTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(std::chrono::milliseconds{10000});
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ConnectTimeoutTests, SetConnectTimeoutTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetConnectTimeout(0L);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ConnectTimeoutTests, SetConnectTimeoutLongTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetConnectTimeout(10000L);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ConnectTimeoutTests, SetChronoConnectTimeoutTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetConnectTimeout(std::chrono::milliseconds{0});
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ConnectTimeoutTests, SetChronoConnectTimeoutLongTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetConnectTimeout(std::chrono::milliseconds{10000});
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(LowSpeedTests, SetLowSpeedTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetLowSpeed({1, 1});
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PayloadTests, SetPayloadTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    session.SetPayload({{"x", "5"}});
    Response response = session.Post();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PayloadTests, SetPayloadLValueTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    Payload payload{{"x", "5"}};
    session.SetPayload(payload);
    Response response = session.Post();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MultipartTests, SetMultipartTest) {
    Url url{server->GetBaseUrl() + "/form_post.html"};
    Session session;
    session.SetUrl(url);
    session.SetMultipart({{"x", "5"}});
    Response response = session.Post();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MultipartTests, SetMultipartValueTest) {
    Url url{server->GetBaseUrl() + "/form_post.html"};
    Session session;
    session.SetUrl(url);
    Multipart multipart{{"x", "5"}};
    session.SetMultipart(multipart);
    Response response = session.Post();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyTests, SetBodyTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    session.SetBody(Body{"x=5"});
    Response response = session.Post();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyTests, SetBodyValueTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    Body body{"x=5"};
    session.SetBody(body);
    Response response = session.Post();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DigestTests, SetDigestTest) {
    Url url{server->GetBaseUrl() + "/digest_auth.html"};
    Session session;
    session.SetUrl(url);
    session.SetAuth({"user", "password", AuthMode::DIGEST});
    Response response = session.Get();
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UserAgentTests, SetUserAgentTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    UserAgent userAgent{"Test User Agent"};
    Session session;
    session.SetUrl(url);
    session.SetUserAgent(userAgent);
    Response response = session.Get();
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(userAgent, response.header["User-Agent"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CookiesTests, BasicCookiesTest) {
    Url url{server->GetBaseUrl() + "/basic_cookies.html"};
    Session session{};
    session.SetUrl(url);
    Response response = session.Get();
    Cookies res_cookies{response.cookies};
    std::string expected_text{"Basic Cookies"};
    cpr::Cookies expectedCookies{
            {"SID", "31d4d96e407aad42", "127.0.0.1", false, "/", true, std::chrono::system_clock::from_time_t(3905119080)},
            {"lang", "en-US", "127.0.0.1", false, "/", true, std::chrono::system_clock::from_time_t(3905119080)},
    };
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
    EXPECT_EQ(expected_text, response.text);
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

TEST(CookiesTests, ClientSetCookiesTest) {
    Url url{server->GetBaseUrl() + "/cookies_reflect.html"};
    {
        Session session{};
        session.SetUrl(url);
        session.SetCookies(Cookies{
                {"SID", "31d4d96e407aad42"},
                {"lang", "en-US"},
        });
        Response response = session.Get();
        std::string expected_text{"SID=31d4d96e407aad42; lang=en-US;"};
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        EXPECT_EQ(expected_text, response.text);
    }
    {
        Session session{};
        session.SetUrl(url);
        Cookies cookie{
                {"SID", "31d4d96e407aad42"},
                {"lang", "en-US"},
        };
        session.SetCookies(cookie);
        Response response = session.Get();
        std::string expected_text{"SID=31d4d96e407aad42; lang=en-US;"};
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        EXPECT_EQ(expected_text, response.text);
    }
}

TEST(CookiesTests, RedirectionWithChangingCookiesTest) {
    Url url{server->GetBaseUrl() + "/redirection_with_changing_cookies.html"};
    {
        Session session{};
        session.SetUrl(url);
        session.SetCookies(Cookies{
                {"SID", "31d4d96e407aad42"},
                {"lang", "en-US"},
        });
        session.SetRedirect(Redirect(0L));
        Response response = session.Get();
        std::string expected_text{"Received cookies are: SID=31d4d96e407aad42; lang=en-US;"};
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        EXPECT_EQ(expected_text, response.text);
    }
    {
        Session session{};
        session.SetUrl(url);
        session.SetRedirect(Redirect(1L));
        Response response = session.Get();
        std::string expected_text{"Received cookies are: lang=en-US; SID=31d4d96e407aad42"};
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        EXPECT_EQ(expected_text, response.text);
    }
    {
        Session session{};
        session.SetUrl(url);
        session.SetCookies(Cookies{
                {"SID", "empty_sid"},
        });
        session.SetRedirect(Redirect(1L));
        Response response = session.Get();
        std::string expected_text{"Received cookies are: lang=en-US; SID=31d4d96e407aad42; SID=empty_sid;"};
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        EXPECT_EQ(expected_text, response.text);
    }
}

TEST(DifferentMethodTests, GetPostTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        Response response = session.Post();
        std::string expected_text{"Header reflect POST"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(DifferentMethodTests, PostGetTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    {
        Response response = session.Post();
        std::string expected_text{"Header reflect POST"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(DifferentMethodTests, GetPostGetTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        Response response = session.Post();
        std::string expected_text{"Header reflect POST"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(DifferentMethodTests, PostGetPostTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    {
        Response response = session.Post();
        std::string expected_text{"Header reflect POST"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        Response response = session.Post();
        std::string expected_text{"Header reflect POST"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(DifferentMethodTests, MultipleGetPostTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    for (size_t i = 0; i < 100; ++i) {
        {
            Response response = session.Get();
            std::string expected_text{"Header reflect GET"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            Response response = session.Post();
            std::string expected_text{"Header reflect POST"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
    }
}

TEST(DifferentMethodTests, MultipleDeleteHeadPutGetPostTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Url urlPost{server->GetBaseUrl() + "/post_reflect.html"};
    Url urlPut{server->GetBaseUrl() + "/put.html"};
    Session session;
    for (size_t i = 0; i < 10; ++i) {
        {
            session.SetUrl(url);
            Response response = session.Delete();
            std::string expected_text{"Header reflect DELETE"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            session.SetUrl(urlPost);
            std::string expectedBody = "a1b2c3Post";
            session.SetBody(expectedBody);
            Response response = session.Post();
            EXPECT_EQ(expectedBody, response.text);
            EXPECT_EQ(urlPost, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            session.SetUrl(url);
            Response response = session.Get();
            std::string expected_text{"Header reflect GET"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            session.SetUrl(urlPut);
            session.SetPayload({{"x", "5"}});
            Response response = session.Put();
            std::string expected_text{
                    "{\n"
                    "  \"x\": 5\n"
                    "}"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(urlPut, response.url);
            EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            session.SetUrl(url);
            Response response = session.Head();
            std::string expected_text{"Header reflect HEAD"};
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
    }
}

TEST(CurlHolderManipulateTests, CustomOptionTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    curl_easy_setopt(session.GetCurlHolder()->handle, CURLOPT_SSL_OPTIONS, CURLSSLOPT_ALLOW_BEAST | CURLSSLOPT_NO_REVOKE);
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        Response response = session.Post();
        std::string expected_text{"Header reflect POST"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(LocalPortTests, SetLocalPortTest) {
    Url url{server->GetBaseUrl() + "/local_port.html"};
    Session session;
    session.SetUrl(url);
    std::uint16_t const local_port = 60252; // beware of HttpServer::GetPort when changing
    std::uint16_t const local_port_range = 50;
    session.SetLocalPort(local_port);
    session.SetLocalPortRange(local_port_range);
    // expected response: body contains port number in specified range
    // NOTE: even when trying up to 50 ports there is the chance that all of them are occupied.
    // It would be possible to also check here for ErrorCode::INTERNAL_ERROR but that somehow seems
    // wrong as then this test would pass in case SetLocalPort does not work at all
    // or in other words: we have to assume that at least one port in the specified range is free.
    Response response = session.Get();
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
    unsigned long port_from_response = std::strtoul(response.text.c_str(), nullptr, 10);
    EXPECT_EQ(errno, 0);
    EXPECT_GE(port_from_response, local_port);
    EXPECT_LE(port_from_response, local_port + local_port_range);
}

TEST(LocalPortTests, SetOptionTest) {
    Url url{server->GetBaseUrl() + "/local_port.html"};
    Session session;
    session.SetUrl(url);
    std::uint16_t const local_port = 60551; // beware of HttpServer::GetPort when changing
    std::uint16_t const local_port_range = 50;
    session.SetOption(LocalPort(local_port));
    session.SetOption(LocalPortRange(local_port_range));
    // expected response: body contains port number in specified range
    // NOTE: even when trying up to 50 ports there is the chance that all of them are occupied.
    // It would be possible to also check here for ErrorCode::INTERNAL_ERROR but that somehow seems
    // wrong as then this test would pass in case SetOption(LocalPort) does not work at all
    // or in other words: we have to assume that at least one port in the specified range is free.
    Response response = session.Get();
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
    unsigned long port_from_response = std::strtoul(response.text.c_str(), nullptr, 10);
    EXPECT_EQ(errno, 0);
    EXPECT_GE(port_from_response, local_port);
    EXPECT_LE(port_from_response, local_port + local_port_range);
}

TEST(LocalPortTests, SetLocalPortTestOccupied) {
    Url url{server->GetBaseUrl() + "/local_port.html"};
    Session session;
    session.SetUrl(url);
    session.SetLocalPort(server->GetPort());
    // expected response: request cannot be made as port is already occupied
    Response response = session.Get();
    EXPECT_EQ(ErrorCode::INTERNAL_ERROR, response.error.code);
}

TEST(LocalPortTests, SetOptionTestOccupied) {
    Url url{server->GetBaseUrl() + "/local_port.html"};
    Session session;
    session.SetUrl(url);
    session.SetOption(LocalPort(server->GetPort()));
    // expected response: request cannot be made as port is already occupied
    Response response = session.Get();
    EXPECT_EQ(ErrorCode::INTERNAL_ERROR, response.error.code);
}

TEST(BasicTests, ReserveResponseString) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetReserveSize(4096);
    Response response = session.Get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_GE(response.text.capacity(), 4096);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, AcceptEncodingTestWithMethodsStringMap) {
    Url url{server->GetBaseUrl() + "/check_accept_encoding.html"};
    Session session;
    session.SetUrl(url);
    session.SetAcceptEncoding({{AcceptEncodingMethods::deflate, AcceptEncodingMethods::gzip, AcceptEncodingMethods::zlib}});
    Response response = session.Get();
    std::string expected_text{"deflate, gzip, zlib"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, AcceptEncodingTestWithMethodsStringMapLValue) {
    Url url{server->GetBaseUrl() + "/check_accept_encoding.html"};
    Session session;
    session.SetUrl(url);
    AcceptEncoding accept_encoding{{AcceptEncodingMethods::deflate, AcceptEncodingMethods::gzip, AcceptEncodingMethods::zlib}};
    session.SetAcceptEncoding(accept_encoding);
    Response response = session.Get();
    std::string expected_text{"deflate, gzip, zlib"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, AcceptEncodingTestWithCostomizedString) {
    Url url{server->GetBaseUrl() + "/check_accept_encoding.html"};
    Session session;
    session.SetUrl(url);
    session.SetAcceptEncoding({{"deflate", "gzip", "zlib"}});
    Response response = session.Get();
    std::string expected_text{"deflate, gzip, zlib"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, AcceptEncodingTestWithCostomizedStringLValue) {
    Url url{server->GetBaseUrl() + "/check_accept_encoding.html"};
    Session session;
    session.SetUrl(url);
    AcceptEncoding accept_encoding{{"deflate", "gzip", "zlib"}};
    session.SetAcceptEncoding(accept_encoding);
    Response response = session.Get();
    std::string expected_text{"deflate, gzip, zlib"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(AsyncRequestsTests, AsyncGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    cpr::AsyncResponse future = session->GetAsync();
    std::string expected_text{"Hello world!"};
    cpr::Response response = future.get();
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(AsyncRequestsTests, AsyncGetMultipleTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};

    std::vector<AsyncResponse> responses;
    std::vector<std::shared_ptr<Session>> sessions;
    for (size_t i = 0; i < 10; ++i) {
        std::shared_ptr<Session> session = std::make_shared<Session>();
        session->SetUrl(url);
        sessions.emplace_back(session);
        responses.emplace_back(session->GetAsync());
    }

    for (cpr::AsyncResponse& future : responses) {
        std::string expected_text{"Hello world!"};
        cpr::Response response = future.get();
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
}

TEST(AsyncRequestsTests, AsyncGetMultipleTemporarySessionTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};

    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        std::shared_ptr<Session> session = std::make_shared<Session>();
        session->SetUrl(url);
        responses.emplace_back(session->GetAsync());
    }

    for (cpr::AsyncResponse& future : responses) {
        std::string expected_text{"Hello world!"};
        cpr::Response response = future.get();
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
}

TEST(AsyncRequestsTests, AsyncGetMultipleReflectTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 100; ++i) {
        std::shared_ptr<Session> session = std::make_shared<Session>();
        session->SetUrl(url);
        session->SetParameters({{"key", std::to_string(i)}});
        responses.emplace_back(session->GetAsync());
    }
    int i = 0;
    for (cpr::AsyncResponse& future : responses) {
        cpr::Response response = future.get();
        std::string expected_text{"Hello world!"};
        Url expected_url{url + "?key=" + std::to_string(i)};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(expected_url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        ++i;
    }
}

TEST(AsyncRequestsTests, AsyncWritebackDownloadTest) {
    std::shared_ptr<Session> session = std::make_shared<Session>();
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    session->SetUrl(url);
    session->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    cpr::AsyncResponse future = session->DownloadAsync(cpr::WriteCallback{write_data, 0});
    cpr::Response response = future.get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(AsyncRequestsTests, AsyncPostTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetPayload({{"x", "5"}});
    cpr::AsyncResponse future = session->PostAsync();
    cpr::Response response = future.get();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(AsyncRequestsTests, AsyncPutTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetPayload({{"x", "5"}});
    cpr::AsyncResponse future = session->PutAsync();
    cpr::Response response = future.get();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(AsyncRequestsTests, AsyncHeadTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    cpr::AsyncResponse future = session->HeadAsync();
    cpr::Response response = future.get();
    std::string expected_text{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(AsyncRequestsTests, AsyncDeleteTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    cpr::AsyncResponse future = session->DeleteAsync();
    cpr::Response response = future.get();
    std::string expected_text{"Header reflect DELETE"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(AsyncRequestsTests, AsyncOptionsTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    cpr::AsyncResponse future = session->OptionsAsync();
    cpr::Response response = future.get();
    std::string expected_text{"Header reflect OPTIONS"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(AsyncRequestsTests, AsyncPatchTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    cpr::AsyncResponse future = session->PatchAsync();
    cpr::Response response = future.get();
    std::string expected_text{"Header reflect PATCH"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CallbackTests, GetCallbackTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    auto future = session->GetCallback([](Response r) { return r; });
    std::this_thread::sleep_for(sleep_time);
    cpr::Response response = future.get();
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CallbackTests, PostCallbackTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    auto future = session->PostCallback([](Response r) { return r; });
    std::this_thread::sleep_for(sleep_time);
    cpr::Response response = future.get();
    std::string expected_text{"Header reflect POST"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CallbackTests, PutCallbackTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetPayload({{"x", "5"}});
    auto future = session->PutCallback([](Response r) { return r; });
    std::this_thread::sleep_for(sleep_time);
    cpr::Response response = future.get();
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CallbackTests, HeadCallbackTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    auto future = session->HeadCallback([](Response r) { return r; });
    std::this_thread::sleep_for(sleep_time);
    cpr::Response response = future.get();
    std::string expected_text{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CallbackTests, DeleteCallbackTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    auto future = session->DeleteCallback([](Response r) { return r; });
    std::this_thread::sleep_for(sleep_time);
    cpr::Response response = future.get();
    std::string expected_text{"Header reflect DELETE"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CallbackTests, OptionsCallbackTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    auto future = session->OptionsCallback([](Response r) { return r; });
    std::this_thread::sleep_for(sleep_time);
    cpr::Response response = future.get();
    std::string expected_text{"Header reflect OPTIONS"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CallbackTests, PatchCallbackTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    auto future = session->PatchCallback([](Response r) { return r; });
    std::this_thread::sleep_for(sleep_time);
    cpr::Response response = future.get();
    std::string expected_text{"Header reflect PATCH"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
