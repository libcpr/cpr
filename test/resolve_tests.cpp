#include <gtest/gtest.h>

#include <string>

#include "cpr/cpr.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(ResolveTests, HelloWorldTest) {
    Url url{"http://www.example.com:" + std::to_string(server->GetPort()) + "/hello.html"};
    Resolve resolve{"www.example.com", "127.0.0.1", {server->GetPort()}};
    Response response = cpr::Get(url, resolve);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(ResolveTests, RedirectToNotResponding) {
    Url url{"http://www.example.com/hello.html"};
    Resolve resolve{"www.example.com", "127.0.0.1", {80}};
    Response response = cpr::Get(url, resolve);
    EXPECT_EQ(0, response.status_code);
    EXPECT_TRUE(response.error.code == ErrorCode::OPERATION_TIMEDOUT || response.error.code == ErrorCode::CONNECTION_FAILURE);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
