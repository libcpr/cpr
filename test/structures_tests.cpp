#include "cpr/cprtypes.h"
#include <gtest/gtest.h>

#include <map>
#include <string>
#include <type_traits>
#include <utility>

#include "cpr/parameters.h"
#include "cpr/payload.h"
#include "cpr/session.h"

using namespace cpr;

namespace {
template <typename T, typename = void>
struct CanSetOption : std::false_type {};

template <typename T>
struct CanSetOption<T, std::void_t<decltype(std::declval<Session&>().SetOption(std::declval<T>()))>> : std::true_type {};
} // namespace

TEST(PayloadTests, UseStringVariableTest) {
    std::string value1 = "hello";
    std::string key2 = "key2";
    Payload payload{{"key1", value1}, {key2, "world"}};

    std::string expected = "key1=hello&key2=world";
    EXPECT_EQ(payload.GetContent(CurlHolder()), expected);
}

TEST(PayloadTests, DisableEncodingTest) {
    std::string key1 = "key1";
    std::string key2 = "key2§$%&/";
    std::string value1 = "hello.,.,";
    std::string value2 = "hello";
    Payload payload{{key1, value1}, {key2, value2}};
    payload.encode = false;

    std::string expected = key1 + '=' + value1 + '&' + key2 + '=' + value2;
    EXPECT_EQ(payload.GetContent(CurlHolder()), expected);
}

TEST(ParametersTests, DefaultConstructorTest) {
    Parameters parameters;
    EXPECT_EQ(std::string{}, parameters.GetContent());
    EXPECT_EQ(std::string{}, parameters.GetContent(CurlHolder()));
    EXPECT_TRUE(parameters.encode);

    parameters.Add(Parameter{"key", "value"});
    EXPECT_EQ(std::string{"key=value"}, parameters.GetContent());
}

TEST(ParametersTests, InitializerListConstructorTest) {
    Parameters parameters{{"key", "first"}, {"empty", ""}, {"key", "second"}};
    std::string expected{"key=first&empty&key=second"};
    EXPECT_EQ(expected, parameters.GetContent());
    EXPECT_EQ(expected, parameters.GetContent(CurlHolder()));
    EXPECT_TRUE(parameters.encode);
}

TEST(ParametersTests, EmptyInitializerListConstructorTest) {
    const std::initializer_list<Parameter> entries{};
    Parameters parameters{entries};
    EXPECT_EQ(std::string{}, parameters.GetContent());
    EXPECT_EQ(std::string{}, parameters.GetContent(CurlHolder()));
}

TEST(ParametersTests, MapConstructorTest) {
    const std::map<std::string, std::string> entries{{"z", "last"}, {"empty", ""}, {"a", "first"}};
    Parameters parameters{entries};
    std::string expected{"a=first&empty&z=last"};
    EXPECT_EQ(expected, parameters.GetContent());
    EXPECT_EQ(expected, parameters.GetContent(CurlHolder()));
    EXPECT_TRUE(parameters.encode);

    parameters.Add(Parameter{"a", "another"});
    EXPECT_EQ(expected + "&a=another", parameters.GetContent());
}

TEST(ParametersTests, EmptyMapConstructorTest) {
    const std::map<std::string, std::string> entries{};
    Parameters parameters{entries};
    EXPECT_EQ(std::string{}, parameters.GetContent());
    EXPECT_EQ(std::string{}, parameters.GetContent(CurlHolder()));
}

TEST(ParametersTests, MapConstructorEncodingTest) {
    const std::map<std::string, std::string> entries{{"a key", "a+b&c=d/%"}, {"empty", ""}};
    Parameters parameters{entries};
    std::string expected{"a%20key=a%2Bb%26c%3Dd%2F%25&empty"};
    EXPECT_EQ(expected, parameters.GetContent(CurlHolder()));

    std::string expected_unencoded{"a key=a+b&c=d/%&empty"};
    EXPECT_EQ(expected_unencoded, parameters.GetContent());
    parameters.encode = false;
    EXPECT_EQ(expected_unencoded, parameters.GetContent(CurlHolder()));
}

TEST(ParametersTests, MapConstructorCopiesInputTest) {
    std::map<std::string, std::string> entries{{"key", "value"}};
    Parameters parameters{entries};
    EXPECT_EQ(std::string{"value"}, entries.at("key"));
    entries.clear();
    EXPECT_EQ(std::string{"key=value"}, parameters.GetContent());
    EXPECT_EQ(std::string{"key=value"}, parameters.GetContent(CurlHolder()));
}

TEST(ParametersTests, MovedMapConstructorTest) {
    const std::string long_value(256, 'x');
    std::map<std::string, std::string> entries{{"z", long_value}, {"empty", ""}, {"a", "first"}};
    Parameters parameters{std::move(entries)};
    std::string expected{"a=first&empty&z=" + long_value};
    EXPECT_EQ(expected, parameters.GetContent());
    EXPECT_EQ(expected, parameters.GetContent(CurlHolder()));
    EXPECT_TRUE(parameters.encode);

    entries.clear();
    parameters.Add(Parameter{"a", "another"});
    EXPECT_EQ(expected + "&a=another", parameters.GetContent());
}

TEST(ParametersTests, EmptyMovedMapConstructorTest) {
    std::map<std::string, std::string> entries{};
    Parameters parameters{std::move(entries)};
    EXPECT_EQ(std::string{}, parameters.GetContent());
    EXPECT_EQ(std::string{}, parameters.GetContent(CurlHolder()));
    EXPECT_TRUE(parameters.encode);
}

TEST(ParametersTests, TemporaryMapConstructorEncodingTest) {
    Parameters parameters{std::map<std::string, std::string>{{"a key", "a+b&c=d/%"}, {"empty", ""}}};
    EXPECT_EQ(std::string{"a%20key=a%2Bb%26c%3Dd%2F%25&empty"}, parameters.GetContent(CurlHolder()));

    std::string expected_unencoded{"a key=a+b&c=d/%&empty"};
    EXPECT_EQ(expected_unencoded, parameters.GetContent());
    parameters.encode = false;
    EXPECT_EQ(expected_unencoded, parameters.GetContent(CurlHolder()));
}

TEST(ParametersTests, MapRequiresExplicitParametersTest) {
    using Map = std::map<std::string, std::string>;
    EXPECT_FALSE((std::is_convertible_v<const Map&, Parameters>));
    EXPECT_FALSE((std::is_convertible_v<Map&&, Parameters>));
    EXPECT_FALSE(CanSetOption<Map&>::value);
    EXPECT_FALSE(CanSetOption<const Map&>::value);
    EXPECT_FALSE(CanSetOption<Map&&>::value);
    EXPECT_TRUE(CanSetOption<Parameters&&>::value);
    EXPECT_TRUE(CanSetOption<const Header&>::value);
}

TEST(ParametersTests, UseStringVariableTest) {
    std::string value1 = "hello";
    std::string key2 = "key2";
    Parameters parameters{{"key1", value1}, {key2, "world"}};

    std::string expected = "key1=hello&key2=world";
    EXPECT_EQ(parameters.GetContent(CurlHolder()), expected);
}

TEST(ParametersTests, DisableEncodingTest) {
    std::string key1 = "key1";
    std::string key2 = "key2§$%&/";
    std::string value1 = "hello.,.,";
    std::string value2 = "hello";
    Parameters parameters{{key1, value1}, {key2, value2}};
    parameters.encode = false;

    std::string expected = key1 + '=' + value1 + '&' + key2 + '=' + value2;
    EXPECT_EQ(parameters.GetContent(CurlHolder()), expected);
}

TEST(ParametersTests, NoCurlHolderTest) {
    std::string key1 = "key1";
    std::string key2 = "key2§$%&/";
    std::string value1 = "hello.,.,";
    std::string value2 = "hello";
    Parameters parameters{{key1, value1}, {key2, value2}};
    const std::string expected = key1 + '=' + value1 + '&' + key2 + '=' + value2;
    EXPECT_EQ(parameters.GetContent(), expected);
}

TEST(UrlToAndFromString, UrlTests) {
    std::string s{"https://github.com/whoshuu/cpr"};
    cpr::Url url = s;
    EXPECT_EQ(s, url.str());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
