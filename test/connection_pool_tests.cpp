#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>

#include "server.h"


static Server* server = new Server();
auto base = server->GetBaseUrl();
auto requests = 100;

TEST(MultipleGetTests, PoolBasicMultipleGetTest) {
    auto url = Url{base + "/hello.html"};
    ConnectionPool pool;
    server->ResetConnectionCount();

    // Without shared connection pool
    for (int i = 0; i < requests; ++i) {
        auto response = cpr::Get(url);
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_EQ(server->GetConnectionCount(), requests);

    // With shared connection pool
    server->ResetConnectionCount();
    for (int i = 0; i < requests; ++i) {
        auto response = cpr::Get(url, pool);
        auto expected_text = std::string{"Hello world!"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_LT(server->GetConnectionCount(), requests);
}

TEST(MultipleGetTests, PoolAsyncGetMultipleTest) {
    auto url = Url{base + "/hello.html"};
    ConnectionPool pool;
    std::vector<AsyncResponse> responses;
    server->ResetConnectionCount();

    // Without shared connection pool
    for (int i = 0; i < requests; ++i) {
        responses.emplace_back(cpr::GetAsync(url));
    }
    for (auto& future : responses) {
        auto expected_text = std::string{"Hello world!"};
        auto response = future.get();
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_EQ(server->GetConnectionCount(), requests);

    // With shared connection pool
    server->ResetConnectionCount();
    responses.clear();
    for (int i = 0; i < requests; ++i) {
        responses.emplace_back(cpr::GetAsync(url, pool));
    }
    for (auto& future : responses) {
        auto expected_text = std::string{"Hello world!"};
        auto response = future.get();

        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
    }
    EXPECT_LT(server->GetConnectionCount(), requests);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
