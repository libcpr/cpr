#include <gtest/gtest.h>

#include <chrono>
#include <string>

#include <cpr/cpr.h>

#include "server.h"

using namespace cpr;

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
    EXPECT_EQ(ErrorCode::OK, response.error.code);
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
    EXPECT_EQ(ErrorCode::OK, response.error.code);
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
    EXPECT_EQ(ErrorCode::OK, response.error.code);
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
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, ZeroMaxRedirectsSuccessTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(MaxRedirects(0));
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, ZeroMaxRedirectsFailureTest) {
    auto url = Url{base + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(MaxRedirects(0));
    auto response = session.Get();
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, OneMaxRedirectsSuccessTest) {
    auto url = Url{base + "/permanent_redirect.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(MaxRedirects(1));
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{base + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, OneMaxRedirectsFailureTest) {
    auto url = Url{base + "/two_redirects.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(MaxRedirects(1));
    auto response = session.Get();
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{base + "/permanent_redirect.html"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(301, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MaxRedirectsTests, TwoMaxRedirectsSuccessTest) {
    auto url = Url{base + "/two_redirects.html"};
    Session session;
    session.SetUrl(url);
    session.SetMaxRedirects(MaxRedirects(2));
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{base + "/hello.html"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MultipleGetTests, BasicMultipleGetTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    for (int i = 0; i < 100; ++i) {
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
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
        auto url = Url{base + "/hello.html"};
        session.SetUrl(url);
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        auto url = Url{base + "/basic.json"};
        session.SetUrl(url);
        auto response = session.Get();
        auto expected_text = std::string{"[\n"
                                         "  {\n"
                                         "    \"first_key\": \"first_value\",\n"
                                         "    \"second_key\": \"second_value\"\n"
                                         "  }\n"
                                         "]"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"application/octet-stream"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, HeaderMultipleGetTest) {
    auto url = Url{base + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    session.SetHeader(Header{{"hello", "world"}});
    for (int i = 0; i < 100; ++i) {
        auto response = session.Get();
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(std::string{"world"}, response.header["hello"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, HeaderChangeMultipleGetTest) {
    auto url = Url{base + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);
    session.SetHeader(Header{{"hello", "world"}});
    {
        auto response = session.Get();
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(std::string{"world"}, response.header["hello"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetHeader(Header{{"key", "value"}});
    {
        auto response = session.Get();
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(std::string{"value"}, response.header["key"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, ParameterMultipleGetTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetParameters({{"hello", "world"}});
    for (int i = 0; i < 100; ++i) {
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(Url{url + "?hello=world"}, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, ParameterChangeMultipleGetTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetParameters({{"hello", "world"}});
    {
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(Url{url + "?hello=world"}, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetUrl(url);
    session.SetParameters({{"key", "value"}});
    {
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(Url{url + "?key=value"}, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, BasicAuthenticationMultipleGetTest) {
    auto url = Url{base + "/basic_auth.html"};
    Session session;
    session.SetUrl(url);
    session.SetAuth(Authentication{"user", "password"});
    for (int i = 0; i < 100; ++i) {
        auto response = session.Get();
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultipleGetTests, BasicAuthenticationChangeMultipleGetTest) {
    auto url = Url{base + "/basic_auth.html"};
    Session session;
    session.SetUrl(url);
    session.SetAuth(Authentication{"user", "password"});
    {
        auto response = session.Get();
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetAuth(Authentication{"user", "bad_password"});
    {
        auto response = session.Get();
        EXPECT_EQ(std::string{}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{}, response.header["content-type"]);
        EXPECT_EQ(401, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    session.SetAuth(Authentication{"bad_user", "password"});
    {
        auto response = session.Get();
        EXPECT_EQ(std::string{}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{}, response.header["content-type"]);
        EXPECT_EQ(401, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(ParameterTests, ParameterSingleTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    Parameters parameters{{"hello", "world"}};
    session.SetParameters(parameters);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterTests, ParameterMultipleTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    Parameters parameters{{"hello", "world"}, {"key", "value"}};
    session.SetParameters(parameters);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?hello=world&key=value"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(TimeoutTests, SetTimeoutTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(0L);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(TimeoutTests, SetTimeoutLongTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(10000L);
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(TimeoutTests, SetChronoTimeoutTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(std::chrono::milliseconds{0});
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(TimeoutTests, SetChronoTimeoutLongTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetTimeout(std::chrono::milliseconds{10000});
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(LowSpeedTests, SetLowSpeedTest) {
    auto url = Url{base + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.SetLowSpeed({1, 1});
    auto response = session.Get();
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PayloadTests, SetPayloadTest) {
    auto url = Url{base + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    session.SetPayload({{"x", "5"}});
    auto response = session.Post();
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PayloadTests, SetPayloadLValueTest) {
    auto url = Url{base + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    auto payload = Payload{{"x", "5"}};
    session.SetPayload(payload);
    auto response = session.Post();
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MultipartTests, SetMultipartTest) {
    auto url = Url{base + "/form_post.html"};
    Session session;
    session.SetUrl(url);
    session.SetMultipart({{"x", "5"}});
    auto response = session.Post();
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(MultipartTests, SetMultipartValueTest) {
    auto url = Url{base + "/form_post.html"};
    Session session;
    session.SetUrl(url);
    auto multipart = Multipart{{"x", "5"}};
    session.SetMultipart(multipart);
    auto response = session.Post();
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyTests, SetBodyTest) {
    auto url = Url{base + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    session.SetBody(Body{"x=5"});
    auto response = session.Post();
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyTests, SetBodyValueTest) {
    auto url = Url{base + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    auto body = Body{"x=5"};
    session.SetBody(body);
    auto response = session.Post();
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DigestTests, SetDigestTest) {
    auto url = Url{base + "/digest_auth.html"};
    Session session;
    session.SetUrl(url);
    session.SetDigest({"user", "password"});
    auto response = session.Get();
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CookiesTests, BasicCookiesTest) {
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
        EXPECT_EQ(ErrorCode::OK, response.error.code);
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
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        EXPECT_EQ(cookies["cookie"], response.cookies["cookie"]);
        EXPECT_EQ(cookies["icecream"], response.cookies["icecream"]);
        EXPECT_EQ(cookies["expires"], response.cookies["expires"]);
    }
}

TEST(CookiesTests, CookiesConstructorTest) {
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
        EXPECT_EQ(ErrorCode::OK, response.error.code);
        cookies = response.cookies;
    }
    {
        cookies = Cookies{{"hello", "world"}, {"my", "another; fake=cookie;"}};
        session.SetCookies(cookies);
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
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
        auto url = Url{base + "/hello.html"};
        session.SetUrl(url);
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        auto url = Url{base + "/url_post.html"};
        session.SetUrl(url);
        session.SetPayload({{"x", "5"}});
        auto response = session.Post();
        auto expected_text = std::string{"{\n"
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
        auto url = Url{base + "/url_post.html"};
        session.SetUrl(url);
        session.SetPayload({{"x", "5"}});
        auto response = session.Post();
        auto expected_text = std::string{"{\n"
                                         "  \"x\": 5\n"
                                         "}"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
        EXPECT_EQ(201, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        auto url = Url{base + "/hello.html"};
        session.SetUrl(url);
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
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
        auto url = Url{base + "/hello.html"};
        session.SetUrl(url);
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        auto url = Url{base + "/url_post.html"};
        session.SetUrl(url);
        session.SetPayload({{"x", "5"}});
        auto response = session.Post();
        auto expected_text = std::string{"{\n"
                                         "  \"x\": 5\n"
                                         "}"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
        EXPECT_EQ(201, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        auto url = Url{base + "/hello.html"};
        session.SetUrl(url);
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
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
        auto url = Url{base + "/url_post.html"};
        session.SetUrl(url);
        session.SetPayload({{"x", "5"}});
        auto response = session.Post();
        auto expected_text = std::string{"{\n"
                                         "  \"x\": 5\n"
                                         "}"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
        EXPECT_EQ(201, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        auto url = Url{base + "/hello.html"};
        session.SetUrl(url);
        auto response = session.Get();
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
    {
        auto url = Url{base + "/url_post.html"};
        session.SetUrl(url);
        session.SetPayload({{"x", "5"}});
        auto response = session.Post();
        auto expected_text = std::string{"{\n"
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
            auto url = Url{base + "/hello.html"};
            session.SetUrl(url);
            auto response = session.Get();
            auto expected_text = std::string{"Hello world!"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            auto url = Url{base + "/url_post.html"};
            session.SetUrl(url);
            session.SetPayload({{"x", "5"}});
            auto response = session.Post();
            auto expected_text = std::string{"{\n"
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
