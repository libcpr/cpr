#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "cpr/cprtypes.h"
#include "cpr/filesystem.h"
#include "cpr/ssl_options.h"

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
    std::string baseDirPath{server->getBaseDirPath()};
    std::string crtPath{baseDirPath + "certificates/"};
    std::string keyPath{baseDirPath + "keys/"};

    SslOptions sslOpts = Ssl(ssl::CaInfo{crtPath + "ca-bundle.crt"}, ssl::CertFile{crtPath + "client.crt"}, ssl::KeyFile{keyPath + "client.key"}, ssl::VerifyPeer{true}, ssl::PinnedPublicKey{keyPath + "server.pub"}, ssl::VerifyHost{true}, ssl::VerifyStatus{false});
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

    SslOptions sslOpts = Ssl(ssl::TLSv1{}, ssl::ALPN{false},
#if SUPPORT_NPN
                             ssl::NPN{false},
#endif // DEBUG
                             ssl::CaInfo{crtPath + "ca-bundle.crt"}, ssl::CertFile{crtPath + "client.crt"}, ssl::KeyFile{keyPath + "client.key"}, ssl::PinnedPublicKey{keyPath + "server.pub"}, ssl::VerifyPeer{true}, ssl::VerifyHost{true}, ssl::VerifyStatus{false});
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

    SslOptions sslOpts = Ssl(ssl::CaInfo{crtPath + "ca-bundle.crt"}, ssl::CertFile{crtPath + "client.crt"}, ssl::KeyFile{keyPath + "client.key"}, ssl::VerifyPeer{true}, ssl::VerifyHost{true}, ssl::VerifyStatus{false});

    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::vector<CertInfo> certInfos = response.GetCertInfos();

    std::string expected_text = "Hello world!";
    std::vector<CertInfo> expectedCertInfos{
            CertInfo{
                    "Subject:CN = test-server",
                    "Issuer:C = GB, O = Example, CN = Sub CA",
                    "Version:2",
                    "Serial Number:acbefc2cde5b900b55548396556765d4",
                    "Signature Algorithm:ED25519",
                    "Public Key Algorithm:ED25519",
                    "X509v3 Authority Key Identifier:9B:B1:9B:21:61:DC:66:2B:3A:AD:ED:84:F1:05:B6:CE:99:82:C1:FC",
                    "X509v3 Basic Constraints:CA:FALSE",
                    "X509v3 Extended Key Usage:TLS Web Client Authentication, TLS Web Server Authentication",
                    "X509v3 Key Usage:Digital Signature, Key Encipherment",
                    "X509v3 Subject Key Identifier:66:47:54:F8:25:97:56:9A:52:56:35:B4:A7:52:60:0C:E7:4F:33:09",
                    "X509v3 Subject Alternative Name:DNS:localhost, IP Address:127.0.0.1, IP Address:0:0:0:0:0:0:0:1",
                    "Start date:May  7 10:18:22 2024 GMT",
                    "Expire date:May  6 10:18:22 2029 GMT",
                    "Signature:6d:63:d9:11:a3:9b:c7:9f:b6:23:12:27:e9:34:e0:a1:a3:20:be:fb:df:80:fe:53:08:9d:8c:e4:82:42:76:c2:55:13:e8:7c:86:83:33:0b:9a:9f:92:2a:3f:de:e9:32:78:c0:b1:bc:3f:42:e9:17:f9:9f:6c:15:35:a3:01:09:",
                    R"(Cert:-----BEGIN CERTIFICATE-----
MIIBtDCCAWagAwIBAgIRAKy+/CzeW5ALVVSDllVnZdQwBQYDK2VwMDAxCzAJBgNV
BAYTAkdCMRAwDgYDVQQKDAdFeGFtcGxlMQ8wDQYDVQQDDAZTdWIgQ0EwHhcNMjQw
NTA3MTAxODIyWhcNMjkwNTA2MTAxODIyWjAWMRQwEgYDVQQDDAt0ZXN0LXNlcnZl
cjAqMAUGAytlcAMhACdLUqJFSyspgGKJiXNlnOLU2dO/TLV+b8aIZNAX7EuVo4Gu
MIGrMB8GA1UdIwQYMBaAFJuxmyFh3GYrOq3thPEFts6ZgsH8MAwGA1UdEwEB/wQC
MAAwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMA4GA1UdDwEB/wQEAwIF
oDAdBgNVHQ4EFgQUZkdU+CWXVppSVjW0p1JgDOdPMwkwLAYDVR0RBCUwI4IJbG9j
YWxob3N0hwR/AAABhxAAAAAAAAAAAAAAAAAAAAABMAUGAytlcANBAG1j2RGjm8ef
tiMSJ+k04KGjIL7734D+UwidjOSCQnbCVRPofIaDMwuan5IqP97pMnjAsbw/QukX
+Z9sFTWjAQk=
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
            std::string search_string = "Identifier:keyid:";
            std::size_t search_index = (*entry_it).find(search_string);
            if (search_index != std::string::npos) {
                (*entry_it).replace(search_index, search_string.length(), "Identifier:");
                search_string = "\n";
                search_index = (*entry_it).find(search_string);
                if (search_index != std::string::npos) {
                    (*entry_it).replace(search_index, search_string.length(), "");
                }
            }
            EXPECT_EQ(*expectedEntry_it, *entry_it);
        }
        std::cout << '\n';
    }
}

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
TEST(SslTests, LoadCertFromBufferTestSimpel) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Url url{server->GetBaseUrl() + "/hello.html"};

    std::string baseDirPath{server->getBaseDirPath()};
    std::string crtPath{baseDirPath + "certificates/"};
    std::string keyPath{baseDirPath + "keys/"};
    std::string certBuffer = loadCertificateFromFile(crtPath + "ca-bundle.crt");
    SslOptions sslOpts = Ssl(ssl::CaBuffer{std::move(certBuffer)}, ssl::CertFile{crtPath + "client.crt"}, ssl::KeyFile{keyPath + "client.key"}, ssl::VerifyPeer{true}, ssl::VerifyHost{true}, ssl::VerifyStatus{false});
    Response response = cpr::Get(url, sslOpts, Timeout{5000}, Verbose{});
    std::string expected_text = "Hello world!";
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code) << response.error.message;
}
#endif

fs::path GetBasePath(const std::string& execPath) {
    return fs::path(fs::path{execPath}.parent_path().string() + "/").make_preferred();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    fs::path baseDirPath = fs::path{GetBasePath(argv[0]).string() + "data/"};
    fs::path serverCertPath = fs::path{baseDirPath}.append("certificates/server.crt");
    fs::path serverKeyPath = fs::path{baseDirPath}.append("keys/server.key");
    server = new HttpsServer(std::move(baseDirPath), std::move(serverCertPath), std::move(serverKeyPath));
    ::testing::AddGlobalTestEnvironment(server);

    return RUN_ALL_TESTS();
}
