#include <gtest/gtest.h>

#include <string>

#include <cpr/cprtypes.h>
#include <cpr/ssl_options.h>

#include "server.h"

static Server* server;
auto base_url = server -> GetBaseUrlSSL();

using namespace cpr;

std::string base_dir;

TEST(SslTests, HelloWorldTest) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto url = Url{base_url + "/hello.html"};
    auto sslOpts = Ssl(ssl::TLSv1{}, ssl::ALPN{false}, ssl::NPN{false}, ssl::CaPath{base_dir},
                       ssl::CertFile{base_dir + "/cert.pem"}, ssl::KeyFile{base_dir + "/key.pem"},
                       ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
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
        base_dir = argv[1];
    }

    server = new Server(base_dir + "/server.pem");

    ::testing::AddGlobalTestEnvironment(server);

    return RUN_ALL_TESTS();
}
