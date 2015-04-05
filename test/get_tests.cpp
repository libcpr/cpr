#include <gtest/gtest.h>

#include <string>

#include <cpr.h>

#include "server.h"


static Server* server = new Server();
auto base = server->GetBaseUrl();

TEST(GetTests, HelloWorldTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Get(url);
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, SingleParameterTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Get(url, Parameters{{"key", "value"}});
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?key=value"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, MultipleParametersTest) {
    auto url = Url{base + "/hello.html"};
    auto response = cpr::Get(url, Parameters{{"key", "value"},
                                             {"hello", "world"},
                                             {"test", "case"}});
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(Url{url + "?hello=world&key=value&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessSingleParameterTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"},
                             Parameters{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessMultipleParametersTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"},
                             Parameters{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(Url{url + "?hello=world&key=value&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessSingleParameterReverseTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"hello", "world"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessMultipleParametersReverseTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"key", "value"},
                                             {"hello", "world"},
                                             {"test", "case"}},
                             Authentication{"user", "password"});

    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(Url{url + "?hello=world&key=value&test=case"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessSingleHeaderTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"},
                             Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessMultipleHeadersTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"},
                             Header{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessSingleHeaderReverseTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationSuccessMultipleHeadersReverseTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"key", "value"}, {"hello", "world"}, {"test", "case"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationNullFailureTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url);
    EXPECT_EQ(std::string{}, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationFailureTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationFailureSingleParameterTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Parameters{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(Url{url + "?hello=world"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationFailureMultipleParametersTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Parameters{{"key", "value"}, {"hello", "world"}, {"test", "case"}});
    EXPECT_EQ(std::string{}, response.text);
    // The query parameters get sorted by alphabetical order by key
    EXPECT_EQ(Url{url + "?hello=world&key=value&test=case"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicJsonTest) {
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
}

TEST(GetTests, HeaderJsonTest) {
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
}

TEST(GetTests, ResourceNotFoundTest) {
    auto url = Url{base + "/error.html"};
    auto response = cpr::Get(url);
    auto expected_text = std::string{"404 Not Found"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(404, response.status_code);
}

TEST(GetTests, HeaderReflectNoneTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url);
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectEmptyTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectSingleTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectMultipleTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectCaseInsensitiveTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"HeLlO", "wOrLd"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hello"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["HELLO"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hElLo"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectNoneParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectEmptyParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{},
                             Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectSingleParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}},
                             Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectMultipleParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}},
                             Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectCaseInsensitiveParametersTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"HeLlO", "wOrLd"}},
                             Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hello"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["HELLO"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hElLo"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectEmptyParametersReverseTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                             Header{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectSingleParametersReverseTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                             Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectMultipleParametersReverseTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                             Header{{"hello", "world"}, {"key", "value"}, {"test", "case"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(std::string{"value"}, response.header["key"]);
    EXPECT_EQ(std::string{"case"}, response.header["test"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, HeaderReflectCaseInsensitiveParametersReverseTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}, {"three", "four"}, {"five", "six"}},
                             Header{{"HeLlO", "wOrLd"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?five=six&one=two&three=four"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hello"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["HELLO"]);
    EXPECT_EQ(std::string{"wOrLd"}, response.header["hElLo"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderAATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Parameters{}, Header{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderABTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"}, Parameters{}, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderACTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Parameters{{"one", "two"}},
                             Header{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderADTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Parameters{{"one", "two"}}, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderAETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Parameters{},
                             Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderAFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"}, Parameters{},
                             Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderAGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Parameters{{"one", "two"}},
                             Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderAHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Parameters{{"one", "two"}}, Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderBATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Header{}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderBBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Header{}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderBCTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Header{},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderBDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Header{},
                             Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderBETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Header{{"hello", "world"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderBFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Header{{"hello", "world"}},
                             Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderBGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Header{{"hello", "world"}},
                             Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderBHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Header{{"hello", "world"}},
                             Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderCATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Authentication{"user", "password"}, Parameters{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderCBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Authentication{"user", "bad_password"}, Parameters{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderCCTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url,  Header{}, Authentication{"user", "password"},
                             Parameters{{"one", "two"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderCDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url,  Header{}, Authentication{"user", "bad_password"},
                             Parameters{{"one", "two"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderCETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Authentication{"user", "password"},
                             Parameters{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderCFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}},
                             Authentication{"user", "bad_password"}, Parameters{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderCGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url,  Header{{"hello", "world"}}, Authentication{"user", "password"},
                             Parameters{{"one", "two"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderCHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url,  Header{{"hello", "world"}},
                             Authentication{"user", "bad_password"}, Parameters{{"one", "two"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderDATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Header{}, Parameters{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderDBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"}, Header{}, Parameters{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderDCTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Header{},
                             Parameters{{"one", "two"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderDDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"}, Header{},
                             Parameters{{"one", "two"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderDETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Header{{"hello", "world"}},
                             Parameters{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderDFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Header{{"hello", "world"}}, Parameters{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderDGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "password"}, Header{{"hello", "world"}},
                             Parameters{{"one", "two"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderDHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Authentication{"user", "bad_password"},
                             Header{{"hello", "world"}}, Parameters{{"one", "two"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderEATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Parameters{}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderEBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Parameters{}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderECTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Parameters{{"one", "two"}}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderEDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{}, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderEETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderEFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderEGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{{"one", "two"}}, Authentication{"user", "password"});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderEHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Header{{"hello", "world"}}, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderFATest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Authentication{"user", "password"}, Header{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderFBTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Authentication{"user", "bad_password"}, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderFCTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "password"}, Header{});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderFDTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"}, Header{});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderFETest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Authentication{"user", "password"}, Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderFFTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{}, Authentication{"user", "bad_password"}, Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderFGTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "password"}, Header{{"hello", "world"}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BasicAuthenticationParameterHeaderFHTest) {
    auto url = Url{base + "/basic_auth.html"};
    auto response = cpr::Get(url, Parameters{{"one", "two"}}, Authentication{"user", "bad_password"}, Header{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(Url{url + "?one=two"}, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(401, response.status_code);
}

TEST(GetTests, SetEmptyHeaderTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Get(url, Header{{"hello", ""}});
    auto expected_text = std::string{"Header reflect"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(std::string{}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
}

TEST(GetTests, BadHostTest) {
    auto url = Url{"http://bad_host/"};
    auto response = cpr::Get(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
