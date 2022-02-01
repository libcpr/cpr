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
#if 0
    const int64_t download_size = 277186;
    cpr::SslOptions sslOpts = cpr::Ssl(cpr::ssl::VerifyPeer{false}, cpr::ssl::VerifyHost{false}, cpr::ssl::VerifyStatus{false});
    cpr::Url url{"https://curl.haxx.se/docs/manpage.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetRange(cpr::Range{0, -1});
    session.SetSslOptions(sslOpts);
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
#endif
}

TEST(DownloadTests, RangeTestLowerLimit) {
#if 0
    const int64_t download_size = 277185;
    cpr::SslOptions sslOpts = cpr::Ssl(cpr::ssl::VerifyPeer{false}, cpr::ssl::VerifyHost{false}, cpr::ssl::VerifyStatus{false});
    cpr::Url url{"https://curl.haxx.se/docs/manpage.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetRange(cpr::Range{1, -1});
    session.SetSslOptions(sslOpts);
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(206, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
#endif
}

TEST(DownloadTests, RangeTestUpperLimit) {
#if 0
    const int64_t download_size = 128;
    cpr::SslOptions sslOpts = cpr::Ssl(cpr::ssl::VerifyPeer{false}, cpr::ssl::VerifyHost{false}, cpr::ssl::VerifyStatus{false});
    cpr::Url url{"https://curl.haxx.se/docs/manpage.html"};
    cpr::Session session;
    session.SetUrl(url);
    session.SetRange(cpr::Range{0, download_size - 1});
    session.SetSslOptions(sslOpts);
    cpr::Response response = session.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(206, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(download_size, response.downloaded_bytes);
#endif
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
