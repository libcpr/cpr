#include "cpr/cpr.h"
#include <cctype>
#include <cstddef>
#include <curl/curl.h>
#include <gtest/gtest.h>
#include <string>


TEST(VersionTests, StringVersionExists) {
#ifndef CPR_VERSION
    EXPECT_TRUE(false);
#endif // CPR_VERSION
}

TEST(VersionTests, StringVersionValid) {
    EXPECT_TRUE(CPR_VERSION != nullptr);
    std::string version = CPR_VERSION;

    // Check if the version string is: '\d+\.\d+\.\d+'
    bool digit = true;
    size_t dotCount = 0;
    for (size_t i = 0; i < version.size(); i++) {
        if (i == 0) {
            EXPECT_TRUE(std::isdigit(version[i]));
        } else if (digit) {
            if (version[i] == '.') {
                digit = false;
                dotCount++;
                continue;
            }
        }
        EXPECT_TRUE(std::isdigit(version[i]));
        digit = true;
    }
    EXPECT_EQ(dotCount, 2);
}

TEST(VersionTests, VersionMajorExists) {
#ifndef CPR_VERSION_MAJOR
    EXPECT_TRUE(false);
#endif // CPR_VERSION_MAJOR
}

TEST(VersionTests, VersionMinorExists) {
#ifndef CPR_VERSION_MINOR
    EXPECT_TRUE(false);
#endif // CPR_VERSION_MINOR
}

TEST(VersionTests, VersionPatchExists) {
#ifndef CPR_VERSION_PATCH
    EXPECT_TRUE(false);
#endif // CPR_VERSION_PATCH
}

TEST(VersionTests, VersionNumExists) {
#ifndef CPR_VERSION_NUM
    EXPECT_TRUE(false);
#endif // CPR_VERSION_NUM
}

#if defined(CPR_ENABLE_SSL)
#if defined(CPR_FORCE_OPENSSL_BACKEND)
TEST(VersionTests, OpenSSLBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    const std::string backend{version->ssl_version};
    // OpenSSL-compatible libraries also use curl's OpenSSL backend.
    EXPECT_TRUE(backend.find("OpenSSL/") == 0 || backend.find("LibreSSL/") == 0 ||
                backend.find("BoringSSL") == 0 || backend.find("AWS-LC/") == 0 ||
                backend.find("quictls/") == 0) << backend;
}
#endif

#if defined(CPR_FORCE_WINSSL_BACKEND)
TEST(VersionTests, WinSSLBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    EXPECT_EQ(std::string{version->ssl_version}.find("Schannel"), 0);
}
#endif

#if defined(CPR_FORCE_DARWINSSL_BACKEND)
TEST(VersionTests, DarwinSSLBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    EXPECT_EQ(std::string{version->ssl_version}.find("SecureTransport"), 0);
}
#endif

#if defined(CPR_FORCE_MBEDTLS_BACKEND)
TEST(VersionTests, MbedTLSBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    EXPECT_EQ(std::string{version->ssl_version}.find("mbedTLS/"), 0);
}
#endif

#if defined(CPR_FORCE_WOLFSSL_BACKEND)
TEST(VersionTests, WolfSSLBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    EXPECT_EQ(std::string{version->ssl_version}.find("wolfSSL/"), 0);
}
#endif
#endif // CPR_ENABLE_SSL


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
