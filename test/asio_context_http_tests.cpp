#include <chrono>
#include <gtest/gtest.h>
#include <memory>

#include <boost/asio/io_context.hpp>

#include "cpr/cpr.h"
#include "httpServer.hpp"

namespace {
cpr::HttpServer* server;
} // namespace

TEST(AsioContextHttpTests, AsyncRequestsUseTheSuppliedGlobalContext) {
    boost::asio::io_context context;
    cpr::async::startup(context, 0, 2);
    const cpr::Url url{server->GetBaseUrl() + "/hello.html"};
    auto request = cpr::GetAsync(url);
    auto session = std::make_shared<cpr::Session>();
    session->SetUrl(url);
    auto sessionRequest = session->GetAsync();
    EXPECT_EQ(request.wait_for(std::chrono::milliseconds(0)), std::future_status::timeout);
    EXPECT_EQ(sessionRequest.wait_for(std::chrono::milliseconds(0)), std::future_status::timeout);
    context.run();
    const auto response = request.get();
    const auto sessionResponse = sessionRequest.get();
    cpr::async::cleanup();
    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.text, "Hello world!");
    EXPECT_EQ(response.error.code, cpr::ErrorCode::OK);
    EXPECT_EQ(sessionResponse.status_code, 200);
    EXPECT_EQ(sessionResponse.text, "Hello world!");
    EXPECT_EQ(sessionResponse.error.code, cpr::ErrorCode::OK);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    server = new cpr::HttpServer();
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
