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

TEST(ResolveTests, RedirectMultiple) {
    Url url1{"http://www.example0.com:" + std::to_string(server->GetPort()) + "/resolve_permanent_redirect.html"};
    Url url2{"http://www.example1.com:" + std::to_string(server->GetPort()) + "/hello.html"};
    Resolve resolve1{"www.example0.com", "127.0.0.1", {server->GetPort()}};
    Resolve resolve2{"www.example1.com", "127.0.0.1", {server->GetPort()}};

    Response response = cpr::Get(url1, std::vector<Resolve>{resolve1, resolve2}, Header{{"RedirectLocation", url2.str()}});

    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url2, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
