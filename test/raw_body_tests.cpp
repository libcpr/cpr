#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include <cpr/cpr.h>
#include <cpr/multipart.h>

#include "server.h"

using namespace cpr;

static Server* server = new Server();
auto base = server->GetBaseUrl();

TEST(BodyPostTests, DefaultUrlEncodedPostTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Body{"x=5"});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, TextUrlEncodedPostTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Body{"x=hello world!!~"});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": hello world!!~\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, TextUrlEncodedNoCopyPostTest) {
    auto url = Url{base + "/url_post.html"};
    auto body = Body{"x=hello world!!~"};
    // body lives through the lifetime of Post, so it doesn't need to be copied
    auto response = cpr::Post(url, body);
    auto expected_text = std::string{"{\n"
                                     "  \"x\": hello world!!~\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, UrlEncodedManyPostTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Body{"x=5&y=13"});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5,\n"
                                     "  \"y\": 13,\n"
                                     "  \"sum\": 18\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, CustomHeaderNumberPostTest) {
    auto url = Url{base + "/json_post.html"};
    auto response = cpr::Post(url, Body{"{\"x\":5}"}, Header{{"Content-Type", "application/json"}});
    auto expected_text = std::string{"{\"x\":5}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, CustomHeaderTextPostTest) {
    auto url = Url{base + "/json_post.html"};
    auto response = cpr::Post(url, Body{"{\"x\":\"hello world!!~\"}"},
                              Header{{"Content-Type", "application/json"}});
    auto expected_text = std::string{"{\"x\":\"hello world!!~\"}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, CustomWrongHeaderPostTest) {
    auto url = Url{base + "/json_post.html"};
    auto response = cpr::Post(url, Body{"{\"x\":5}"}, Header{{"Content-Type", "text/plain"}});
    auto expected_text = std::string{"Unsupported Media Type"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(415, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, UrlPostBadHostTest) {
    auto url = Url{"http://bad_host/"};
    auto response = cpr::Post(url, Body{"hello=world"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::HOST_RESOLUTION_FAILURE, response.error.code);
}

TEST(BodyPostTests, StringMoveBodyTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Body{std::string{"x=5"}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
