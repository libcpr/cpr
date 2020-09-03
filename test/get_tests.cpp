#include <gtest/gtest.h>

#include <string>

#include "cpr/cpr.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(BasicTests, HelloWorldTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Get(url);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, TimeoutTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Get(url, Timeout{0L});
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, BasicJsonTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    Response response = cpr::Get(url);
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

TEST(BasicTests, ResourceNotFoundTest) {
    Url url{server->GetBaseUrl() + "/error.html"};
    Response response = cpr::Get(url);
    std::string expected_text{"Not Found"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(404, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, BadHostTest) {
    Url url{"http://bad_host/"};
    Response response = cpr::Get(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::HOST_RESOLUTION_FAILURE, response.error.code);
}

TEST(CookiesTests, SingleCookieTest) {
    Url url{server->GetBaseUrl() + "/basic_cookies.html"};
    Cookies cookies{{"hello", "world"}, {"my", "another; fake=cookie;"}};
    Response response = cpr::Get(url, cookies);
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

TEST(CookiesTests, EmptyCookieTest) {
    Url url{server->GetBaseUrl() + "/empty_cookies.html"};
    Response response = cpr::Get(url);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
    EXPECT_EQ("", response.cookies["cookie"]);
    EXPECT_EQ("", response.cookies["icecream"]);
}

TEST(CookiesTests, CheckBasicCookieTest) {
    // server validates whether the cookies are indeed present
    Url url{server->GetBaseUrl() + "/check_cookies.html"};
    Cookies cookies{{"cookie", "chocolate"}, {"icecream", "vanilla"}};
    Response response = cpr::Get(url, cookies);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CookiesTests, V1CookieTest) {
    Url url{server->GetBaseUrl() + "/v1_cookies.html"};
    Response response = cpr::Get(url);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
    Cookies cookies = response.cookies;
    EXPECT_EQ("\"value with spaces (v1 cookie)\"", cookies["cookie"]);
}

TEST(CookiesTests, CheckV1CookieTest) {
    // server validates whether the cookie is indeed present
    Url url{server->GetBaseUrl() + "/check_v1_cookies.html"};
    Cookies cookies{{"cookie", "\"value with spaces (v1 cookie)\""}};
    Response response = cpr::Get(url, cookies);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterTests, SingleParameterTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Parameters parameters{{"key", "value"}};
    Response response = cpr::Get(url, parameters);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterTests, SingleParameterOnlyKeyTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Parameters parameters{{"key", ""}};
    Response response = cpr::Get(url, parameters);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(ParameterTests, MultipleParametersTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response =
            cpr::Get(url, Parameters{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterTests, MultipleDynamicParametersTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Parameters parameters{{"key", "value"}};
    parameters.Add({"hello", "world"});
    parameters.Add({"test", "case"});
    Response response = cpr::Get(url, parameters);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationTests, BasicAuthenticationSuccessTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationTests, BasicDigestSuccessTest) {
    Url url{server->GetBaseUrl() + "/digest_auth.html"};
    Response response = cpr::Get(url, Digest{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAthenticationParameterTests, BasicAuthenticationSuccessSingleParameterTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "password"}, Parameters{{"hello", "world"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationSuccessMultipleParametersTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "password"},
                     Parameters{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationSuccessSingleParameterReverseTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Parameters{{"hello", "world"}}, Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationSuccessMultipleParametersReverseTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Parameters{{"key", "value"}, {"hello", "world"}, {"test", "case"}},
                     Authentication{"user", "password"});

    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationHeaderTests, BasicAuthenticationSuccessSingleHeaderTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "password"}, Header{{"hello", "world"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationHeaderTests, BasicAuthenticationSuccessMultipleHeadersTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "password"},
                                 Header{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationHeaderTests, BasicAuthenticationSuccessSingleHeaderReverseTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Header{{"hello", "world"}}, Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationHeaderTests, BasicAuthenticationSuccessMultipleHeadersReverseTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Header{{"key", "value"}, {"hello", "world"}, {"test", "case"}},
                     Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationTests, BasicAuthenticationNullFailureTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url);
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationTests, BasicAuthenticationFailureTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationFailureSingleParameterTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "bad_password"}, Parameters{{"hello", "world"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationFailureMultipleParametersTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "bad_password"},
                     Parameters{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderJsonTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    Response response = cpr::Get(url, Header{{"content-type", "application/json"}});
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

TEST(HeaderTests, HeaderReflectNoneTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Get(url);
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderReflectEmptyTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Get(url, Header{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderReflectSingleTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderReflectMultipleTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response =
            cpr::Get(url, Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderReflectCaseInsensitiveTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Get(url, Header{{"HeLlO", "wOrLd"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hello"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["HELLO"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hElLo"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, SetEmptyHeaderTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Get(url, Header{{"hello", ""}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectNoneParametersTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response =
            cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectEmptyParametersTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response =
            cpr::Get(url, Header{}, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectSingleParametersTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}},
                                 Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectMultipleParametersTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response =
            cpr::Get(url, Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}},
                     Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectCaseInsensitiveParametersTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response = cpr::Get(url, Header{{"HeLlO", "wOrLd"}},
                                 Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hello"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["HELLO"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hElLo"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectEmptyParametersReverseTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response =
            cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}}, Header{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectSingleParametersReverseTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response =
            cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                     Header{{"hello", "world"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectMultipleParametersReverseTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response =
            cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                     Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectCaseInsensitiveParametersReverseTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Response response =
            cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                     Header{{"HeLlO", "wOrLd"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hello"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["HELLO"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hElLo"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAATest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "password"}, Parameters{}, Header{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderABTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "bad_password"}, Parameters{}, Header{});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderACTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "password"}, Parameters{{"one", "two"}}, Header{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderADTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "bad_password"},
                                 Parameters{{"one", "two"}}, Header{});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAETest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "password"}, Parameters{},
                                 Header{{"hello", "world"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAFTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "bad_password"}, Parameters{},
                                 Header{{"hello", "world"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAGTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "password"},
                                 Parameters{{"one", "two"}}, Header{{"hello", "world"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAHTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "bad_password"},
                                 Parameters{{"one", "two"}}, Header{{"hello", "world"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBATest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{}, Header{}, Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBBTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Parameters{}, Header{}, Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBCTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Parameters{{"one", "two"}}, Header{}, Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBDTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{{"one", "two"}}, Header{},
                                 Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBETest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{}, Header{{"hello", "world"}},
                                 Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBFTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{}, Header{{"hello", "world"}},
                                 Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBGTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{{"one", "two"}}, Header{{"hello", "world"}},
                                 Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBHTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{{"one", "two"}}, Header{{"hello", "world"}},
                                 Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCATest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{}, Authentication{"user", "password"}, Parameters{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCBTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Header{}, Authentication{"user", "bad_password"}, Parameters{});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCCTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Header{}, Authentication{"user", "password"}, Parameters{{"one", "two"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCDTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{}, Authentication{"user", "bad_password"},
                                 Parameters{{"one", "two"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCETest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}},
                                 Authentication{"user", "password"}, Parameters{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCFTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}},
                                 Authentication{"user", "bad_password"}, Parameters{});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCGTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}},
                                 Authentication{"user", "password"}, Parameters{{"one", "two"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCHTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Header{{"hello", "world"}}, Authentication{"user", "bad_password"},
                     Parameters{{"one", "two"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDATest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "password"}, Header{}, Parameters{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDBTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "bad_password"}, Header{}, Parameters{});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDCTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Authentication{"user", "password"}, Header{}, Parameters{{"one", "two"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDDTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "bad_password"}, Header{},
                                 Parameters{{"one", "two"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDETest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "password"},
                                 Header{{"hello", "world"}}, Parameters{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDFTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "bad_password"},
                                 Header{{"hello", "world"}}, Parameters{});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDGTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "password"},
                                 Header{{"hello", "world"}}, Parameters{{"one", "two"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDHTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Authentication{"user", "bad_password"},
                                 Header{{"hello", "world"}}, Parameters{{"one", "two"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEATest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{}, Parameters{}, Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEBTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Header{}, Parameters{}, Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderECTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Header{}, Parameters{{"one", "two"}}, Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEDTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{}, Parameters{{"one", "two"}},
                                 Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEETest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{},
                                 Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEFTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{},
                                 Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEGTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{{"one", "two"}},
                                 Authentication{"user", "password"});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEHTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{{"one", "two"}},
                                 Authentication{"user", "bad_password"});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFATest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{}, Authentication{"user", "password"}, Header{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFBTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Parameters{}, Authentication{"user", "bad_password"}, Header{});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFCTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "password"}, Header{});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFDTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{{"one", "two"}},
                                 Authentication{"user", "bad_password"}, Header{});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFETest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{}, Authentication{"user", "password"},
                                 Header{{"hello", "world"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFFTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{}, Authentication{"user", "bad_password"},
                                 Header{{"hello", "world"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ("text/plain", response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFGTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response = cpr::Get(url, Parameters{{"one", "two"}},
                                 Authentication{"user", "password"}, Header{{"hello", "world"}});
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFHTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Response response =
            cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"},
                     Header{{"hello", "world"}});
    EXPECT_EQ("Unauthorized", response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(GetRedirectTests, RedirectTest) {
    Url url{server->GetBaseUrl() + "/temporary_redirect.html"};
    Response response = cpr::Get(url, false); // This should be turned into an object
    std::string expected_text{"Moved Temporarily"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(302, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(GetRedirectTests, ZeroMaxRedirectsTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Get(url, MaxRedirects(0));
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, RequestBodyTest) {
    Url url{server->GetBaseUrl() + "/body_get.html"};
    Body body{"message=abc123"};
    Response response = cpr::Get(url, body);
    std::string expected_text{"abc123"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
