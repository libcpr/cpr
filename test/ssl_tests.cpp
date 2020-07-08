#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include <cpr/cprtypes.h>
#include <cpr/ssl_options.h>

#include "httpsServer.hpp"

using namespace cpr;

static HttpsServer* server;

TEST(SslTests, HelloWorldTest) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string baseDirPath = server->getBaseDirPath();
    SslOptions sslOpts = Ssl(
            ssl::TLSv1{}, ssl::ALPN{false}, ssl::NPN{false}, ssl::CaPath{baseDirPath + "ca.cer"},
            ssl::CertFile{baseDirPath + "client.cer"}, ssl::KeyFile{baseDirPath + "client.key"},
            ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
}

/**
 * We should replace this with a C++17 filesystem call,
 * once we have updated to >= C++17.
 **/
std::string getBasePath(const std::string& execPath) {
    std::string path = execPath.substr(0, execPath.find_last_of("\\/") + 1);
    
    // If Windows convert paths from "D:/cpr/build/bin/Release/client.cer" to "D:\cpr\build\bin\Release\client.cer":
#ifdef _WIN32
    std::cout << "Converting Unix path to Windows path...\n";
    std::replace(path.begin(), path.end(), '\\', '/');
    std::cout << "Result path: " << path << '\n';
#endif
    return path;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    std::string baseDirPath = getBasePath(argv[0]);
    std::string serverCertPath = baseDirPath + "server.cer";
    std::string serverKeyPath = baseDirPath + "server.key";
    server = new HttpsServer(std::move(baseDirPath), std::move(serverCertPath),
                             std::move(serverKeyPath));
    ::testing::AddGlobalTestEnvironment(server);

    return RUN_ALL_TESTS();
}
