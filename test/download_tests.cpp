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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
