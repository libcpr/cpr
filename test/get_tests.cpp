#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "server.h"

using namespace cpr;

static Server* server = new Server();
auto base = server->GetBaseUrl();

TEST(BasicTests, HelloWorldTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Get(url);
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, TimeoutTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Get(url, Timeout{0L});
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, BasicJsonTest) {
    auto url = Url{base + "/basic.json"};
    auto response = cpr::Get(url);
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

TEST(BasicTests, ResourceNotFoundTest) {
    auto url = Url{base + "/error.html"};
    auto response = cpr::Get(url);
    auto expected_text = std::string{"404 Not Found\n"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(404, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicTests, BadHostTest) {
    auto url = Url{"http://bad_host/"};
    auto response = cpr::Get(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::HOST_RESOLUTION_FAILURE, response.error.code);
}

TEST(BasicTests, RequestBodyTest) {
    auto url = Url{base + "/body_get.html"};
    auto body = Body{"message=abc123"};
    auto response = cpr::Get(url, body);
    auto expected_text = std::string{"abc123"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CookiesTests, SingleCookieTest) {
    auto url = Url{base + "/basic_cookies.html"};
    auto cookies = Cookies{{"hello", "world"}, {"my", "another; fake=cookie;"}};
    auto response = cpr::Get(url, cookies);
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

TEST(CookiesTests, CheckBasicCookieTest) {
    // server validates whether the cookies are indeed present
    auto url = Url{base + "/check_cookies.html"};
    auto cookies = Cookies{{"cookie", "chocolate"}, {"icecream", "vanilla"}};
    auto response = cpr::Get(url, cookies);
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CookiesTests, V1CookieTest) {
    auto url = Url{base + "/v1_cookies.html"};
    auto response = cpr::Get(url);
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
    auto cookies = response.cookies;
    EXPECT_EQ("\"value with spaces (v1 cookie)\"", cookies["cookie"]);
}

TEST(CookiesTests, CheckV1CookieTest) {
    // server validates whether the cookie is indeed present
    auto url = Url{base + "/check_v1_cookies.html"};
    auto cookies = Cookies{{"cookie", "\"value with spaces (v1 cookie)\""}};
    auto response = cpr::Get(url, cookies);
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterTests, SingleParameterTest) {
    auto url = Url{base + "/hello.html"};
    auto parameters = Parameters{{"key", "value"}};
    auto response = cpr::Get(url, parameters);
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterTests, SingleParameterOnlyKeyTest) {
    auto url = Url{base + "/hello.html"};
    auto parameters = Parameters{{"key", ""}};
    auto response = cpr::Get(url, parameters);
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(ParameterTests, MultipleParametersTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Get(url, Parameters{{"key", "value"},
                                             {"hello", "world"},
                                             {"test", "case"}});
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterTests, MultipleDynamicParametersTest) {
    auto url = Url{base + "/hello.html"};
    auto parameters = Parameters{{"key", "value"}};
    parameters.AddParameter({"hello", "world"});
    parameters.AddParameter({"test", "case"});
    auto response = cpr::Get(url, parameters);
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationTests, BasicAuthenticationSuccessTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationTests, BasicDigestSuccessTest) {
    auto url = Url{base + "/digest_auth.html"};
    auto response = cpr::Get(url, Digest{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAthenticationParameterTests, BasicAuthenticationSuccessSingleParameterTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"},
                             Parameters{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationSuccessMultipleParametersTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"},
                             Parameters{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationSuccessSingleParameterReverseTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"hello", "world"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationSuccessMultipleParametersReverseTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"key", "value"},
                                             {"hello", "world"},
                                             {"test", "case"}},
                             Authentication{"user", "password"});

    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationHeaderTests, BasicAuthenticationSuccessSingleHeaderTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"},
                             Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationHeaderTests, BasicAuthenticationSuccessMultipleHeadersTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"},
                             Header{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    auto expected_text = std::string{"Header reflect GET"};
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
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationHeaderTests, BasicAuthenticationSuccessMultipleHeadersReverseTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"key", "value"}, {"hello", "world"}, {"test", "case"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
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
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationTests, BasicAuthenticationFailureTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationFailureSingleParameterTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Parameters{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterTests, BasicAuthenticationFailureMultipleParametersTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Parameters{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?key=value&hello=world&test=case"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderJsonTest) {
    auto url = Url{base + "/basic.json"};
    auto response = cpr::Get(url, Header{{"content-type", "application/json"}});
    auto expected_text = std::string{"[\n"
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
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url);
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderReflectEmptyTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderReflectSingleTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(HeaderTests, HeaderReflectMultipleTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}});
    auto expected_text = std::string{"Header reflect GET"};
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
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"HeLlO", "wOrLd"}});
    auto expected_text = std::string{"Header reflect GET"};
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
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", ""}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectNoneParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectEmptyParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{},
                             Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectSingleParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}},
                             Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectMultipleParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}},
                             Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect GET"};
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
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"HeLlO", "wOrLd"}},
                             Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect GET"};
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
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                             Header{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectSingleParametersReverseTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                             Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two&three=four&five=six"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ParameterHeaderTests, HeaderReflectMultipleParametersReverseTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                             Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}});
    auto expected_text = std::string{"Header reflect GET"};
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
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                             Header{{"HeLlO", "wOrLd"}});
    auto expected_text = std::string{"Header reflect GET"};
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
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Parameters{}, Header{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderABTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"}, Parameters{}, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderACTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Parameters{{"one", "two"}},
                             Header{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderADTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Parameters{{"one", "two"}}, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Parameters{},
                             Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"}, Parameters{},
                             Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Parameters{{"one", "two"}},
                             Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderAHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Parameters{{"one", "two"}}, Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Header{}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Header{}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBCTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Header{},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Header{},
                             Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Header{{"hello", "world"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Header{{"hello", "world"}},
                             Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Header{{"hello", "world"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderBHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Header{{"hello", "world"}},
                             Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Authentication{"user", "password"}, Parameters{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Authentication{"user", "bad_password"}, Parameters{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCCTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url,  Header{}, Authentication{"user", "password"},
                             Parameters{{"one", "two"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url,  Header{}, Authentication{"user", "bad_password"},
                             Parameters{{"one", "two"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Authentication{"user", "password"},
                             Parameters{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}},
                             Authentication{"user", "bad_password"}, Parameters{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url,  Header{{"hello", "world"}}, Authentication{"user", "password"},
                             Parameters{{"one", "two"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderCHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url,  Header{{"hello", "world"}},
                             Authentication{"user", "bad_password"}, Parameters{{"one", "two"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Header{}, Parameters{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"}, Header{}, Parameters{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDCTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Header{},
                             Parameters{{"one", "two"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"}, Header{},
                             Parameters{{"one", "two"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Header{{"hello", "world"}},
                             Parameters{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Header{{"hello", "world"}}, Parameters{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Header{{"hello", "world"}},
                             Parameters{{"one", "two"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderDHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Header{{"hello", "world"}}, Parameters{{"one", "two"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Parameters{}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Parameters{}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderECTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Parameters{{"one", "two"}}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{{"one", "two"}}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderEHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Authentication{"user", "password"}, Header{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Authentication{"user", "bad_password"}, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFCTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "password"}, Header{});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"}, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Authentication{"user", "password"}, Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Authentication{"user", "bad_password"}, Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "password"}, Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BasicAuthenticationParameterHeaderTests, BasicAuthenticationParameterHeaderFHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"}, Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(GetRedirectTests, RedirectTest) {
    auto url = Url{base + "/temporary_redirect.html"};
    auto response = cpr::Get(url, false); // This should be turned into an object
    auto expected_text = std::string{"Found"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(302, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(GetRedirectTests, ZeroMaxRedirectsTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Get(url, MaxRedirects(0));
    auto expected_text = std::string{"Hello world!"};
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
