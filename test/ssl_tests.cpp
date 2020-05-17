#include <gtest/gtest.h>

#include <string>

#include <cpr/cprtypes.h>
#include <cpr/ssl_options.h>

#include "server.h"

static Server* server;
auto base = server -> GetBaseUrlSSL();

using namespace cpr;

std::string basedir;

TEST(SslTests, HelloWorldTest) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto url = Url{base + "/hello.html"};
    auto sslOpts = Ssl(ssl::CertFile{basedir + "/cert.pem"}, ssl::KeyFile{basedir + "/key.pem"},
                       ssl::MaxTLSv1_1{}, ssl::VerifyStatus{false});
    auto response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    auto expected_text = std::string{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    if (argc > 1) {
        basedir = argv[1];
    }

    server = new Server(basedir + "/server.pem");

    ::testing::AddGlobalTestEnvironment(server);

    return RUN_ALL_TESTS();
}
