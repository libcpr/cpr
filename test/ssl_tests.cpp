#include <gtest/gtest.h>

#include <string>

#include <cpr/cprtypes.h>
#include <cpr/ssl_options.h>

#include "httpsServer.hpp"

using namespace cpr;

static HttpsServer* server;

TEST(SslTests, HelloWorldTest) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::string url = Url{server->GetBaseUrl() + "/hello.html"};
    std::string baseDirPath = server->getBaseDirPath();
    SslOptions sslOpts =
            Ssl(ssl::TLSv1{}, ssl::ALPN{false}, ssl::NPN{false}, ssl::CaPath{baseDirPath},
                ssl::CertFile{baseDirPath + "/cert.pem"}, ssl::KeyFile{baseDirPath + "/key.pem"},
                ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    std::string baseDirPath = argv[1];
    server = new HttpsServer(std::move(baseDirPath), "server.pem", "key.pem");
    ::testing::AddGlobalTestEnvironment(server);

    return RUN_ALL_TESTS();
}
