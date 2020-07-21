#include <gtest/gtest.h>

#include <chrono>
#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

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
    session.SetRedirect(false);
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
    session.SetRedirect(false);
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
    session.SetMaxRedirects(MaxRedirects(0));
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
    session.SetMaxRedirects(MaxRedirects(0));
    Response response = session.Get();
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, OneMaxRedirectsSuccessTest) {
    Url url{server->GetBaseUrl() + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(MaxRedirects(1));
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
    session.SetMaxRedirects(MaxRedirects(1));
    Response response = session.Get();
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/permanent_redirect.html"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, TwoMaxRedirectsSuccessTest) {
    Url url{server->GetBaseUrl() + "/two_redirects.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(MaxRedirects(2));
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
    for (int i = 0; i < 100; ++i) {
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
    for (int i = 0; i < 100; ++i) {
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
    for (int i = 0; i < 100; ++i) {
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
    session.SetAuth(Authentication{"user", "password"});
    for (int i = 0; i < 100; ++i) {
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
    session.SetAuth(Authentication{"user", "password"});
    {
        Response response = session.Get();
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetAuth(Authentication{"user", "bad_password"});
    {
        Response response = session.Get();
        EXPECT_EQ(std::string{"Unauthorized"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
        EXPECT_EQ(401, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetAuth(Authentication{"bad_user", "password"});
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
    session.SetDigest({"user", "password"});
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
    Cookies cookies;

    {
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        cookies = response.cookies;
    }
    {
        cookies["hello"] = "world";
        cookies["my"] = "another; fake=cookie;"; // This is url encoded
        session.SetCookies(cookies);
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        EXPECT_EQ(cookies["cookie"], response.cookies["cookie"]);
        EXPECT_EQ(cookies["icecream"], response.cookies["icecream"]);
        EXPECT_EQ(cookies["expires"], response.cookies["expires"]);
    }
}

TEST(CookiesTests, CookiesConstructorTest) {
    Url url{server->GetBaseUrl() + "/basic_cookies.html"};
    Session session{};
    session.SetUrl(url);
    Cookies cookies;

    {
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        cookies = response.cookies;
    }
    {
        cookies = Cookies{{"hello", "world"}, {"my", "another; fake=cookie;"}};
        session.SetCookies(cookies);
        Response response = session.Get();
        std::string expected_text{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        cookies = response.cookies;
        EXPECT_EQ(cookies["cookie"], response.cookies["cookie"]);
        EXPECT_EQ(cookies["icecream"], response.cookies["icecream"]);
        EXPECT_EQ(cookies["expires"], response.cookies["expires"]);
    }
}

TEST(DifferentMethodTests, GetPostTest) {
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
        Url url{server->GetBaseUrl() + "/url_post.html"};
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
}

TEST(DifferentMethodTests, PostGetTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/url_post.html"};
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
}

TEST(DifferentMethodTests, GetPostGetTest) {
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
        Url url{server->GetBaseUrl() + "/url_post.html"};
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
}

TEST(DifferentMethodTests, PostGetPostTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/url_post.html"};
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
        Url url{server->GetBaseUrl() + "/url_post.html"};
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
}

TEST(DifferentMethodTests, MultipleGetPostTest) {
    Session session;
    for (int i = 0; i < 100; ++i) {
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
            Url url{server->GetBaseUrl() + "/url_post.html"};
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
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
