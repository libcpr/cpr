#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include <cpr/cpr.h>
#include <cpr/multipart.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(BodyPostTests, DefaultUrlEncodedPostTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = cpr::Post(url, Body{"x=5"});
    std::string expected_text = "{\n  \"x\": 5\n}";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, TextUrlEncodedPostTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = cpr::Post(url, Body{"x=hello world!!~"});
    std::string expected_text{
            "{\n"
            "  \"x\": hello world!!~\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, TextUrlEncodedNoCopyPostTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Body body{"x=hello world!!~"};
    // body lives through the lifetime of Post, so it doesn't need to be copied
    Response response = cpr::Post(url, body);
    std::string expected_text{
            "{\n"
            "  \"x\": hello world!!~\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, UrlEncodedManyPostTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = cpr::Post(url, Body{"x=5&y=13"});
    std::string expected_text{
            "{\n"
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
    Url url{server->GetBaseUrl() + "/json_post.html"};
    Response response =
            cpr::Post(url, Body{"{\"x\":5}"}, Header{{"Content-Type", "application/json"}});
    std::string expected_text{"{\"x\":5}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, CustomHeaderTextPostTest) {
    Url url{server->GetBaseUrl() + "/json_post.html"};
    Response response = cpr::Post(url, Body{"{\"x\":\"hello world!!~\"}"},
                                  Header{{"Content-Type", "application/json"}});
    std::string expected_text{"{\"x\":\"hello world!!~\"}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, CustomWrongHeaderPostTest) {
    Url url{server->GetBaseUrl() + "/json_post.html"};
    Response response = cpr::Post(url, Body{"{\"x\":5}"}, Header{{"Content-Type", "text/plain"}});
    std::string expected_text{"Unsupported Media Type"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(415, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(BodyPostTests, UrlPostBadHostTest) {
    Url url{"http://bad_host/"};
    Response response = cpr::Post(url, Body{"hello=world"});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::HOST_RESOLUTION_FAILURE, response.error.code);
}

TEST(BodyPostTests, StringMoveBodyTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = cpr::Post(url, Body{std::string{"x=5"}});
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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
