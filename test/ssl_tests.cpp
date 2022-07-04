#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <cpr/cprtypes.h>
#include <cpr/ssl_options.h>

#include "httpsServer.hpp"


using namespace cpr;

static HttpsServer* server;

static std::string caCertPath;
static std::string serverPubKeyPath;
static std::string clientKeyPath;
static std::string clientCertPath;

std::string loadCertificateFromFile(const std::string certPath) {
    std::ifstream certFile(certPath);
    std::stringstream buffer;
    buffer << certFile.rdbuf();
    return buffer.str();
}

TEST(SslTests, HelloWorldTestSimpel) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};
    SslOptions sslOpts = Ssl(ssl::CaPath{std::move(caCertPath)}, ssl::CertFile{std::move(clientCertPath)}, ssl::KeyFile{std::move(clientKeyPath)}, ssl::VerifyPeer{false}, ssl::PinnedPublicKey{std::move(serverPubKeyPath)}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
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
    SslOptions sslOpts = Ssl(ssl::TLSv1{}, ssl::ALPN{false}, ssl::NPN{false}, ssl::CaPath{std::move(caCertPath)}, ssl::CertFile{std::move(clientCertPath)}, ssl::KeyFile{std::move(clientKeyPath)}, ssl::PinnedPublicKey{std::move(serverPubKeyPath)}, ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
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

    SslOptions sslOpts = Ssl(ssl::CaPath{std::move(caCertPath)}, ssl::CertFile{std::move(clientCertPath)}, ssl::KeyFile{std::move(clientKeyPath)}, ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;

    std::vector<std::string> certInfo = response.GetCertInfo();
    EXPECT_EQ(certInfo.size(), 1);
    std::string expected_certInfo = "Subject:CN = test-server";
    EXPECT_EQ(certInfo[0], expected_certInfo);
}

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
TEST(SslTests, LoadCertFromBufferTestSimpel) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};

    std::string certBuffer = loadCertificateFromFile(caCertPath);
    SslOptions sslOpts = Ssl(ssl::CaBuffer{std::move(certBuffer)}, ssl::CertFile{std::move(clientCertPath)}, ssl::KeyFile{std::move(clientKeyPath)}, ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
}
#endif

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

    std::string baseDirPath = getBasePath(argv[0]) + "data/";
    std::string keyDirPath = baseDirPath + "keys/";
    std::string certDirPath = baseDirPath + "certificates/";

    caCertPath = baseDirPath + "certificates/root-ca.cer";
    serverPubKeyPath = keyDirPath + "server.pub";
    clientKeyPath = keyDirPath + "client.key";
    clientCertPath = certDirPath + "client.crt";

    server = new HttpsServer(std::move(baseDirPath), certDirPath + "server.crt", keyDirPath + "server.key");
    ::testing::AddGlobalTestEnvironment(server);

    return RUN_ALL_TESTS();
}
