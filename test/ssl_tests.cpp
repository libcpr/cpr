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

std::string loadCertificateFromFile(const std::string certPath) {
    std::ifstream certFile(certPath);
    std::stringstream buffer;
    buffer << certFile.rdbuf();
    return buffer.str();
}

TEST(SslTests, HelloWorldTestSimpel) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string baseDirPath{server->getBaseDirPath()};
    std::string crtPath{baseDirPath + "certificates/"};
    std::string keyPath{baseDirPath + "keys/"};

    SslOptions sslOpts = Ssl(ssl::CaPath{crtPath + "root-ca.crt"}, ssl::CertFile{crtPath + "client.crt"}, ssl::KeyFile{keyPath + "client.key"}, ssl::VerifyPeer{false}, ssl::PinnedPublicKey{keyPath + "server.pub"}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
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
    std::string baseDirPath{server->getBaseDirPath()};
    std::string crtPath{baseDirPath + "certificates/"};
    std::string keyPath{baseDirPath + "keys/"};

    SslOptions sslOpts = Ssl(ssl::TLSv1{}, ssl::ALPN{false}, ssl::NPN{false}, ssl::CaPath{crtPath + "root-ca.crt"}, ssl::CertFile{crtPath + "client.crt"}, ssl::KeyFile{keyPath + "client.key"}, ssl::PinnedPublicKey{keyPath + "server.pub"}, ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
}

TEST(SslTests, GetCertInfos) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string baseDirPath{server->getBaseDirPath()};
    std::string crtPath{baseDirPath + "certificates/"};
    std::string keyPath{baseDirPath + "keys/"};

    SslOptions sslOpts = Ssl(ssl::CaPath{crtPath + "root-ca.crt"}, ssl::CertFile{crtPath + "client.crt"}, ssl::KeyFile{keyPath + "client.key"}, ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});

    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::vector<CertInfo> certInfos = response.GetCertInfos();

    std::string expected_text = "Hello world!";
    std::vector<CertInfo> expectedCertInfos{
            CertInfo{
                    "Subject:CN = test-server",
                    "Issuer:C = GB, O = Example, CN = Root CA",
                    "Version:2",
                    "Serial Number:28c252871ec62a626a98006b0bf2888f",
                    "Signature Algorithm:ED25519",
                    "Public Key Algorithm:ED25519",
                    "X509v3 Subject Alternative Name:DNS:localhost, IP Address:127.0.0.1, IP Address:0:0:0:0:0:0:0:1",
                    "X509v3 Subject Key Identifier:39:C1:81:38:01:DC:55:38:E5:2F:4E:7A:D0:4C:84:7B:B7:27:D3:AF",
                    "X509v3 Authority Key Identifier:keyid:E4:F2:F3:85:0E:B7:85:75:84:76:E3:43:D1:B6:9D:14:B8:E2:A4:B7\n",
                    "Start date:Jun 29 11:33:07 2022 GMT",
                    "Expire date:Jun 28 11:33:07 2027 GMT",
                    "Signature:2e:0d:a1:0d:f5:90:77:e9:eb:84:7d:80:63:63:4d:8a:eb:d9:23:57:1f:21:2a:ed:81:b4:a8:58:b9:00:1b:cb:5c:90:1b:33:6b:f6:ec:42:20:63:54:d6:60:ee:37:14:1b:1c:95:0b:33:ea:67:29:d4:cc:d9:7e:34:fd:47:04:",
                    R"(Cert:-----BEGIN CERTIFICATE-----
MIIBdTCCASegAwIBAgIQKMJShx7GKmJqmABrC/KIjzAFBgMrZXAwMTELMAkGA1UE
BhMCR0IxEDAOBgNVBAoMB0V4YW1wbGUxEDAOBgNVBAMMB1Jvb3QgQ0EwHhcNMjIw
NjI5MTEzMzA3WhcNMjcwNjI4MTEzMzA3WjAWMRQwEgYDVQQDDAt0ZXN0LXNlcnZl
cjAqMAUGAytlcAMhAI64JU5RjfdEG1KQMxS5DQWkiGlKIQO7ye4mNFq9QleTo3Aw
bjAsBgNVHREEJTAjgglsb2NhbGhvc3SHBH8AAAGHEAAAAAAAAAAAAAAAAAAAAAEw
HQYDVR0OBBYEFDnBgTgB3FU45S9OetBMhHu3J9OvMB8GA1UdIwQYMBaAFOTy84UO
t4V1hHbjQ9G2nRS44qS3MAUGAytlcANBAC4NoQ31kHfp64R9gGNjTYrr2SNXHyEq
7YG0qFi5ABvLXJAbM2v27EIgY1TWYO43FBsclQsz6mcp1MzZfjT9RwQ=
-----END CERTIFICATE-----
)",
            },
    };
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
    EXPECT_EQ(1, certInfos.size());
    for (auto certInfo_it = certInfos.begin(), expectedCertInfo_it = expectedCertInfos.begin(); certInfo_it != certInfos.end() && expectedCertInfo_it != expectedCertInfos.end(); certInfo_it++, expectedCertInfo_it++) {
        for (auto entry_it = (*certInfo_it).begin(), expectedEntry_it = (*expectedCertInfo_it).begin(); entry_it != (*certInfo_it).end() && expectedEntry_it != (*expectedCertInfo_it).end(); entry_it++, expectedEntry_it++) {
            EXPECT_EQ(*expectedEntry_it, *entry_it);
        }
        std::cout << std::endl;
    }
}

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
TEST(SslTests, LoadCertFromBufferTestSimpel) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};

    std::string baseDirPath{server->getBaseDirPath()};
    std::string crtPath{baseDirPath + "certificates/"};
    std::string keyPath{baseDirPath + "keys/"};
    std::string certBuffer = loadCertificateFromFile(crtPath + "root-ca.crt");
    SslOptions sslOpts = Ssl(ssl::CaBuffer{std::move(certBuffer)}, ssl::CertFile{crtPath + "client.crt"}, ssl::KeyFile{keyPath + "client.key"}, ssl::VerifyPeer{false}, ssl::VerifyHost{false}, ssl::VerifyStatus{false});
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

    server = new HttpsServer(std::move(baseDirPath), baseDirPath + "certificates/server.crt", baseDirPath + "keys/server.key");
    ::testing::AddGlobalTestEnvironment(server);

    return RUN_ALL_TESTS();
}
