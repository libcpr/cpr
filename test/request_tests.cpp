#include <gtest/gtest.h>

#include <string>

#include "cpr/api.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(RequestTests, GetTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Parameters parameters{{"key", "value"}};
    Header header{{"hello", "world"}};
    Response response = Request<HTTPMethod::kGet>(url, parameters, header);
    EXPECT_EQ(std::string{"Header reflect GET"}, response.text);
    EXPECT_EQ(Url{url + "?key=value"}, response.url);
    EXPECT_EQ(std::string{"world"}, response.header["hello"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RequestTests, PostTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = Request<HTTPMethod::kPost>(url, Payload{{"x", "5"}});
    EXPECT_EQ(std::string{"{\n  \"x\": 5\n}"}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RequestTests, PutTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    Response response = Request<HTTPMethod::kPut>(url, Payload{{"x", "5"}});
    EXPECT_EQ(std::string{"{\n  \"x\": 5\n}"}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RequestTests, DeleteTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    Response response = Request<HTTPMethod::kDelete>(url);
    EXPECT_EQ(std::string{"Delete success"}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RequestTests, PatchTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    Response response = Request<HTTPMethod::kPatch>(url, Payload{{"x", "5"}});
    EXPECT_EQ(std::string{"{\n  \"x\": 5\n}"}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RequestTests, HeadTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = Request<HTTPMethod::kHead>(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(RequestTests, OptionsTest) {
    Url url{server->GetBaseUrl() + "/"};
    Response response = Request<HTTPMethod::kOptions>(url);
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
