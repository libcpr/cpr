#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "cpr/cpr.h"
#include "cpr/filesystem.h"

#include "cpr/api.h"
#include "cpr/response.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

bool write_data(const std::string_view& /*data*/, intptr_t /*userdata*/) {
    return true;
}

TEST(AsyncTests, AsyncGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    cpr::AsyncResponse future = cpr::GetAsync(url);
    std::string expected_text{"Hello world!"};
    cpr::Response response = future.get();
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(response.primary_ip, "127.0.0.1");
    EXPECT_EQ(response.primary_port, server->GetPort());
}

TEST(AsyncTests, AsyncGetMultipleTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::GetAsync(url));
    }
    for (cpr::AsyncResponse& future : responses) {
        std::string expected_text{"Hello world!"};
        cpr::Response response = future.get();

// Sometimes on apple specific operating systems, this test fails with socket errors leading to could not connect.
// This is a known issue on macOS and not related to cpr.
#ifdef __APPLE__
        if (response.error.code == cpr::ErrorCode::OK) {
#endif
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
            EXPECT_EQ(200, response.status_code);
#ifdef __APPLE__
        } else {
            EXPECT_EQ(response.error.code, cpr::ErrorCode::COULDNT_CONNECT);
        }
#endif
    }
}

TEST(AsyncTests, AsyncGetMultipleReflectTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 100; ++i) {
        Parameters p{{"key", std::to_string(i)}};
        responses.emplace_back(cpr::GetAsync(url, p));
    }
    int i = 0;
    for (cpr::AsyncResponse& future : responses) {
        cpr::Response response = future.get();

// Sometimes on apple specific operating systems, this test fails with socket errors leading to could not connect.
// This is a known issue on macOS and not related to cpr.
#ifdef __APPLE__
        if (response.error.code == cpr::ErrorCode::OK) {
#endif
            std::string expected_text{"Hello world!"};
            EXPECT_EQ(expected_text, response.text);
            Url expected_url{url + "?key=" + std::to_string(i)};
            EXPECT_EQ(expected_url, response.url);
            EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
            EXPECT_EQ(200, response.status_code);
#ifdef __APPLE__
        } else {
            EXPECT_EQ(response.error.code, cpr::ErrorCode::COULDNT_CONNECT);
        }
#endif
        ++i;
    }
}

TEST(AsyncTests, AsyncDownloadTest) {
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::AsyncResponse future = cpr::DownloadAsync(fs::path{"/tmp/aync_download"}, url, cpr::Header{{"Accept-Encoding", "gzip"}}, cpr::WriteCallback{write_data, 0});
    cpr::Response response = future.get();
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
