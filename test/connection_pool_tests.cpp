#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();
const size_t NUM_REQUESTS = 10;

TEST(MultipleGetTests, PoolBasicMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    ConnectionPool pool;
    server->ResetConnectionCount();

    // Without shared connection pool - make 10 sequential requests
    for (size_t i = 0; i < NUM_REQUESTS; ++i) {
        Response response = cpr::Get(url);
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_EQ(server->GetConnectionCount(), NUM_REQUESTS);

    // With shared connection pool - make 10 sequential requests
    server->ResetConnectionCount();
    for (size_t i = 0; i < NUM_REQUESTS; ++i) {
        Response response = cpr::Get(url, pool);
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_LT(server->GetConnectionCount(), NUM_REQUESTS);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
