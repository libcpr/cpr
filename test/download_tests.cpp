#include <cstddef>
#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "cpr/api.h"
#include "cpr/callback.h"
#include "cpr/cprtypes.h"
#include "cpr/session.h"
#include "httpServer.hpp"


static cpr::HttpServer* server = new cpr::HttpServer();

bool write_data(std::string /*data*/, intptr_t /*userdata*/) {
    return true;
}

TEST(DownloadTests, DownloadGzip) {
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::Session session;
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetUrl(url);
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(DownloadTests, RangeTestWholeFile) {
    const int64_t download_size = 9;
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetRange(cpr::Range{0, -1});
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
}

TEST(DownloadTests, RangeTestLowerLimit) {
    const int64_t download_size = 8;
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetRange(cpr::Range{1, -1});
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(206, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
}

TEST(DownloadTests, RangeTestUpperLimit) {
    const int64_t download_size = 6;
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetRange(cpr::Range{0, download_size - 1});
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(206, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
}

TEST(DownloadTests, RangeTestLowerAndUpperLimit) {
    const int64_t download_size = 2;
    const int64_t start_from = 2;
    const int64_t finish_at = start_from + download_size - 1;
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetRange(cpr::Range{start_from, finish_at});
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(206, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
}

TEST(DownloadTests, RangeTestMultipleRangesSet) {
    const int64_t num_parts = 2;
    const int64_t download_size = num_parts * (26 /*content range*/ + 4 /*\n*/) + ((num_parts + 1) * 15 /*boundary*/) + 2 /*--*/ + 6 /*data*/;
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetMultiRange(cpr::MultiRange{cpr::Range{0, 3}, cpr::Range{5, 6}});
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(206, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
}

TEST(DownloadTests, RangeTestMultipleRangesOption) {
    const int64_t num_parts = 3;
    const int64_t download_size = num_parts * (26 /*content range*/ + 4 /*\n*/) + ((num_parts + 1) * 15 /*boundary*/) + 2 /*--*/ + 7 /*data*/;
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetOption(cpr::MultiRange{cpr::Range{0, 2}, cpr::Range{4, 5}, cpr::Range{7, 8}});
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(206, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
