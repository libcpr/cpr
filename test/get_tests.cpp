#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include <cpr.h>

#include "server.h"


static Server* server;
auto base = Url{"http://localhost:8080"};

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

int main(int argc, char** argv) {
    server = new Server();
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
