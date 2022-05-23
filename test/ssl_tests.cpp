#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <cpr/cprtypes.h>
#include <cpr/ssl_options.h>

#include "httpsServer.hpp"

#include <fstream>
#include <iostream>

using namespace cpr;

static HttpsServer* server;

std::shared_ptr<std::vector<char>> loadCertificateIntoBuffer(const std::string certPath) {
    std::ifstream certFile(certPath, std::ios::binary | std::ios::ate);
    std::streamsize size = certFile.tellg();
    certFile.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (certFile.read(buffer.data(), size)) {
        return std::make_shared<std::vector<char>>(buffer.begin(), buffer.end());
    }
    return nullptr;
}

TEST(SslTests, HelloWorldTestSimpel) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string baseDirPath = server->getBaseDirPath();
    SslOptions sslOpts = Ssl(ssl::CaPath{baseDirPath + "ca.cer"}, ssl::CertFile{baseDirPath + "client.cer"}, ssl::KeyFile{baseDirPath + "client.key"}, ssl::VerifyPeer{false}, ssl::PinnedPublicKey{baseDirPath + "server.pubkey"}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
}

TEST(SslTests, HelloWorldTestFull) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string baseDirPath = server->getBaseDirPath();
    SslOptions sslOpts = Ssl(ssl::TLSv1{}, ssl::ALPN{false}, ssl::NPN{false}, ssl::CaPath{baseDirPath + "ca.cer"}, ssl::CertFile{baseDirPath + "client.cer"}, ssl::KeyFile{baseDirPath + "client.key"}, ssl::PinnedPublicKey{baseDirPath + "server.pubkey"}, ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
}

TEST(SslTests, GetCertInfo) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string baseDirPath = server->getBaseDirPath();
    SslOptions sslOpts = Ssl(ssl::CaPath{baseDirPath + "ca.cer"}, ssl::CertFile{baseDirPath + "client.cer"}, ssl::KeyFile{baseDirPath + "client.key"}, ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;

    std::vector<std::string> certInfo = response.GetCertInfo();
    EXPECT_EQ(certInfo.size(), 1);
    std::string expected_certInfo = "Subject:C = XX, L = Default City, O = Default Company Ltd";
    EXPECT_EQ(certInfo[0], expected_certInfo);
}

TEST(SslTests, LoadCertFromBufferTestSimpel) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string baseDirPath = server->getBaseDirPath();
    std::cout << "baseDirPath: " << baseDirPath << "\n";

    std::shared_ptr<std::vector<char>> clientCertBuffer = loadCertificateIntoBuffer(baseDirPath + "client.cer");
    EXPECT_TRUE(clientCertBuffer != nullptr);


    std::cout << "Clint certificate at: " << clientCertBuffer << "\n";
    for (char c : *clientCertBuffer)
        std::cout << c;

    cpr::SslOptions sslOpts = cpr::Ssl(ssl::CertBuffer{std::move(clientCertBuffer)}, ssl::KeyFile{baseDirPath + "client.key"});
    cpr::Response r = cpr::Get(cpr::Url{"https://www.httpbin.org/get"}, sslOpts);
    std::cout << r.text << "\n";
    std::cout << r.status_code << "\n";
    std::cout << (int) r.error.code << "\n";

    SslOptions sslOpts_2 = Ssl(ssl::CaPath{baseDirPath + "ca.cer"}, ssl::CertBuffer{std::move(clientCertBuffer)}, ssl::KeyFile{baseDirPath + "client.key"}, ssl::VerifyPeer{false}, ssl::PinnedPublicKey{baseDirPath + "server.pubkey"}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts_2, Timeout{5000}, Verbose{});
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

    // If Windows convert paths from "D:/cpr/build/bin/Release/client.cer" to
    // "D:\cpr\build\bin\Release\client.cer":
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
    server = new HttpsServer(std::move(baseDirPath), std::move(serverCertPath), std::move(serverKeyPath));
    ::testing::AddGlobalTestEnvironment(server);

    return RUN_ALL_TESTS();
}
