#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();
const size_t NUM_REQUESTS = 100;

TEST(MultipleGetTests, PoolBasicMultipleGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    ConnectionPool pool;
    server->ResetConnectionCount();

    // Without shared connection pool
    for (int i = 0; i < NUM_REQUESTS; ++i) {
        Response response = cpr::Get(url);
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_EQ(server->GetConnectionCount(), NUM_REQUESTS);

    // With shared connection pool
    server->ResetConnectionCount();
    for (int i = 0; i < NUM_REQUESTS; ++i) {
        Response response = cpr::Get(url, pool);
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_LT(server->GetConnectionCount(), NUM_REQUESTS);
}

TEST(MultipleGetTests, PoolAsyncGetMultipleTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    ConnectionPool pool;
    std::vector<AsyncResponse> responses;
    server->ResetConnectionCount();

    // Without shared connection pool
    responses.reserve(NUM_REQUESTS);
    for (int i = 0; i < NUM_REQUESTS; ++i) {
        responses.emplace_back(cpr::GetAsync(url));
    }
    for (AsyncResponse& future : responses) {
        Response response = future.get();
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_EQ(server->GetConnectionCount(), NUM_REQUESTS);

    // With shared connection pool
    server->ResetConnectionCount();
    responses.clear();
    for (int i = 0; i < NUM_REQUESTS; ++i) {
        responses.emplace_back(cpr::GetAsync(url, pool));
    }
    for (AsyncResponse& future : responses) {
        Response response = future.get();

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
