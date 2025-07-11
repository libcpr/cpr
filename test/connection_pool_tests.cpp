#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <thread>
#include <chrono>

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

TEST(MultipleGetTests, PoolAsyncGetMultipleTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    ConnectionPool pool;
    std::vector<AsyncResponse> responses;
    server->ResetConnectionCount();

    const size_t NUM_BATCHES = 2;
    const size_t BATCH_SIZE = NUM_REQUESTS / 2;  // 5 requests per batch

    // Without shared connection pool - two batches with 10ms sleep
    responses.reserve(NUM_REQUESTS);
    
    for (size_t batch = 0; batch < NUM_BATCHES; ++batch) {
        for (size_t i = 0; i < BATCH_SIZE; ++i) {
            responses.emplace_back(cpr::GetAsync(url));
        }
        
        // Sleep between batches but not after the last batch
        if (batch != NUM_BATCHES - 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    
    // Wait for all responses
    for (AsyncResponse& future : responses) {
        Response response = future.get();
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_EQ(server->GetConnectionCount(), NUM_REQUESTS);

    // With shared connection pool - same two-batch approach
    server->ResetConnectionCount();
    responses.clear();
    responses.reserve(NUM_REQUESTS);
    
    for (size_t batch = 0; batch < NUM_BATCHES; ++batch) {
        for (size_t i = 0; i < BATCH_SIZE; ++i) {
            responses.emplace_back(cpr::GetAsync(url, pool));
        }
        
        // Sleep between batches but not after the last batch
        if (batch != NUM_BATCHES - 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    // Wait for all responses
    for (AsyncResponse& future : responses) {
        Response response = future.get();
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    
    // With connection pooling, should use fewer connections than requests
    EXPECT_LT(server->GetConnectionCount(), NUM_REQUESTS);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
} 