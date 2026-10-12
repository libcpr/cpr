#include <gtest/gtest.h>

#include <curl/curl.h>
#include <fstream>
#include <memory>
#include <optional>
#include <string>

#include <cpr/cpr.h>
#include <cpr/filesystem.h>

#include "cpr/api.h"
#include "cpr/file.h"
#include "cpr/multipart.h"
#include "httpServer.hpp"

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, cert-err58-cpp)
cpr::HttpServer* server = new cpr::HttpServer();
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::optional<cpr::fs::path> baseDirPath{std::nullopt};

cpr::fs::path GetBasePath(const std::string& execPath) {
    return cpr::fs::path(cpr::fs::path{execPath}.parent_path().string() + "/").make_preferred();
}


cpr::fs::path UnicodeTestPath(const char* utf8_name, const wchar_t* wide_name) {
#ifdef _WIN32
    (void) utf8_name;
    return *baseDirPath / wide_name;
#else
    (void) wide_name;
    return *baseDirPath / utf8_name;
#endif
}
} // namespace


TEST(FileUploadTests, AsciiFileName) {
    // Ensure 'baseDirPath' has been set
    ASSERT_NE(baseDirPath, std::nullopt);

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    cpr::fs::path filePath = *baseDirPath / "test_file.txt";

    cpr::Multipart mp{{cpr::Part("file_name", cpr::File(filePath.string()))}};
    cpr::Url url{server->GetBaseUrl() + "/post_file_upload.html"};
    cpr::Response response = cpr::Post(url, mp);

    // Expected file content
    std::ifstream ifs(filePath.c_str(), std::ios::binary);
    ASSERT_TRUE(ifs.is_open());
    ASSERT_GT(cpr::fs::file_size(filePath), 0U);
    std::string expected_text = "{\n  \"file_name\": \"" + filePath.filename().string() + "=" + std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())) + "\"\n}";

    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(cpr::Url{server->GetBaseUrl() + "/post_file_upload.html"}, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(FileUploadTests, NonAsciiFileName) {
#ifdef _WIN32
    if ((curl_version_info(CURLVERSION_NOW)->features & CURL_VERSION_UNICODE) == 0) {
        GTEST_SKIP() << "Path-based Unicode uploads require a Unicode-enabled libcurl; streaming is tested separately";
    }
#endif
    // Ensure 'baseDirPath' has been set
    ASSERT_NE(baseDirPath, std::nullopt);

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    const std::string filename{u8"test_file_hello_äüöp_2585.txt"};
    cpr::fs::path filePath = UnicodeTestPath(filename.c_str(), L"test_file_hello_äüöp_2585.txt");

    cpr::Multipart mp{{cpr::Part("file_name", cpr::File(baseDirPath->string() + filename))}};
    cpr::Url url{server->GetBaseUrl() + "/post_file_upload.html"};
    cpr::Response response = cpr::Post(url, mp);

    // Expected file content
    std::ifstream ifs(filePath.c_str(), std::ios::binary);
    ASSERT_TRUE(ifs.is_open());
    ASSERT_GT(cpr::fs::file_size(filePath), 0U);
    std::string expected_text = "{\n  \"file_name\": \"" + filename + "=" + std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())) + "\"\n}";

    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(cpr::Url{server->GetBaseUrl() + "/post_file_upload.html"}, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(FileUploadTests, ChineseFileName) {
#ifdef _WIN32
    if ((curl_version_info(CURLVERSION_NOW)->features & CURL_VERSION_UNICODE) == 0) {
        GTEST_SKIP() << "Path-based Unicode uploads require a Unicode-enabled libcurl; streaming is tested separately";
    }
#endif
    // Ensure 'baseDirPath' has been set
    ASSERT_NE(baseDirPath, std::nullopt);

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    const std::string filename{u8"test_file_hello_äüöp_2585_你好.txt"};
    cpr::fs::path filePath = UnicodeTestPath(filename.c_str(), L"test_file_hello_äüöp_2585_你好.txt");

    cpr::Multipart mp{{cpr::Part("file_name", cpr::File(baseDirPath->string() + filename))}};
    cpr::Url url{server->GetBaseUrl() + "/post_file_upload.html"};
    cpr::Response response = cpr::Post(url, mp);

    // Expected file content
    std::ifstream ifs(filePath.c_str(), std::ios::binary);
    ASSERT_TRUE(ifs.is_open());
    ASSERT_GT(cpr::fs::file_size(filePath), 0U);
    std::string expected_text = "{\n  \"file_name\": \"" + filename + "=" + std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())) + "\"\n}";

    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(cpr::Url{server->GetBaseUrl() + "/post_file_upload.html"}, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(FileUploadTests, StreamChineseFileName) {
    ASSERT_NE(baseDirPath, std::nullopt);
    const std::string filename{u8"test_file_hello_äüöp_2585_你好.txt"};
    const cpr::fs::path path = UnicodeTestPath(filename.c_str(), L"test_file_hello_äüöp_2585_你好.txt");
    auto file = std::make_shared<std::ifstream>(path.c_str(), std::ios::binary);
    ASSERT_TRUE(file->is_open());
    ASSERT_GT(cpr::fs::file_size(path), 0U);
    const std::string expected_content{std::istreambuf_iterator<char>{*file}, std::istreambuf_iterator<char>{}};
    ASSERT_FALSE(expected_content.empty());
    file->clear();
    file->seekg(0);
    cpr::MultipartStream stream{cpr::ReadCallback{static_cast<cpr::cpr_off_t>(cpr::fs::file_size(path)),
                                                  [file](char* buffer, size_t& size, intptr_t) {
                                                      file->read(buffer, static_cast<std::streamsize>(size));
                                                      size = static_cast<size_t>(file->gcount());
                                                      return !file->bad() && (!file->fail() || file->eof());
                                                  }},
                                filename};
    const auto response = cpr::Post(cpr::Url{server->GetBaseUrl() + "/post_file_upload.html"}, cpr::Multipart{{cpr::Part{"file", std::move(stream)}}});
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ("{\n  \"file\": \"" + filename + "=" + expected_content + "\"\n}", response.text);
}

int main(int argc, char** argv) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    baseDirPath = std::make_optional<cpr::fs::path>(cpr::fs::path{GetBasePath(argv[0]).string() + "data/"});

    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
