#include <gtest/gtest.h>

#include <fstream>
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
} // namespace

cpr::fs::path GetBasePath(const std::string& execPath) {
    return cpr::fs::path(cpr::fs::path{execPath}.parent_path().string() + "/").make_preferred();
}


TEST(FileUploadTests, AsciiFileName) {
    // Ensure 'baseDirPath' has been set
    EXPECT_NE(baseDirPath, std::nullopt);

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    cpr::fs::path filePath = *baseDirPath / "test_file.txt";

    cpr::Multipart mp{{cpr::Part("file_name", cpr::File(filePath.string()))}};
    cpr::Url url{server->GetBaseUrl() + "/post_file_upload.html"};
    cpr::Response response = cpr::Post(url, mp);

    // Expected file content
    std::ifstream ifs(filePath.string());
    std::string expected_text = "{\n  \"file_name\": \"" + filePath.filename().string() + "=" + std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())) + "\"\n}";

    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(cpr::Url{server->GetBaseUrl() + "/post_file_upload.html"}, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(FileUploadTests, NonAsciiFileName) {
    // Ensure 'baseDirPath' has been set
    EXPECT_NE(baseDirPath, std::nullopt);

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    cpr::fs::path filePath = *baseDirPath / "test_file_hello_äüöp_2585.txt";

    cpr::Multipart mp{{cpr::Part("file_name", cpr::File(filePath.string()))}};
    cpr::Url url{server->GetBaseUrl() + "/post_file_upload.html"};
    cpr::Response response = cpr::Post(url, mp);

    // Expected file content
    std::ifstream ifs(filePath.string());
    std::string expected_text = "{\n  \"file_name\": \"" + filePath.filename().string() + "=" + std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())) + "\"\n}";

    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(cpr::Url{server->GetBaseUrl() + "/post_file_upload.html"}, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(FileUploadTests, ChineseFileName) {
    // Ensure 'baseDirPath' has been set
    EXPECT_NE(baseDirPath, std::nullopt);

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    cpr::fs::path filePath = *baseDirPath / "test_file_hello_äüöp_2585.txt";

    cpr::Multipart mp{{cpr::Part("file_name", cpr::File(filePath.string()))}};
    cpr::Url url{server->GetBaseUrl() + "/post_file_upload.html"};
    cpr::Response response = cpr::Post(url, mp);

    // Expected file content
    std::ifstream ifs(filePath.string());
    std::string expected_text = "{\n  \"file_name\": \"" + filePath.filename().string() + "=" + std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())) + "\"\n}";

    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(cpr::Url{server->GetBaseUrl() + "/post_file_upload.html"}, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    baseDirPath = std::make_optional<cpr::fs::path>(cpr::fs::path{GetBasePath(argv[0]).string() + "data/"});

    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
