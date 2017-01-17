#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <fstream>
#include <string>

#include <cpr/cpr.h>

#include "cpr/multipart.h"
#include "server.h"

using namespace cpr;

static Server* server = new Server();
auto base = server->GetBaseUrl();


TEST(UrlEncodedPostTests, UrlPostSingleTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Payload{{"x", "5"}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, UrlPostAddPayloadPair) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "1"}};
    payload.AddPair({"y", "2"});
    auto response = cpr::Post(url, Payload(payload));
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 1,\n"
                                     "  \"y\": 2,\n"
                                     "  \"sum\": 3\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
}

TEST(UrlEncodedPostTests, UrlPostPayloadIteratorTest) {
    auto url = Url{base + "/url_post.html"};
    std::vector<Pair> payloadData;
    payloadData.push_back({"x", "1"});
    payloadData.push_back({"y", "2"});
    auto response = cpr::Post(url, Payload(payloadData.begin(), payloadData.end()));
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 1,\n"
                                     "  \"y\": 2,\n"
                                     "  \"sum\": 3\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
}

TEST(UrlEncodedPostTests, UrlPostEncodeTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Payload{{"x", "hello world!!~"}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": hello world!!~\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, UrlPostEncodeNoCopyTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "hello world!!~"}};
    // payload lives through the lifetime of Post, so it doesn't need to be copied
    auto response = cpr::Post(url, payload);
    auto expected_text = std::string{"{\n"
                                     "  \"x\": hello world!!~\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, UrlPostManyTest) {
    auto url = Url{base + "/url_post.html"};
    auto response = cpr::Post(url, Payload{{"x", 5}, {"y", 13}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5,\n"
                                     "  \"y\": 13,\n"
                                     "  \"sum\": 18\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, UrlPostBadHostTest) {
    auto url = Url{"http://bad_host/"};
    auto response = cpr::Post(url, Payload{{"hello", "world"}});
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{}, response.header["content-type"]);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::HOST_RESOLUTION_FAILURE, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostSingleTest) {
    auto url = Url{base + "/form_post.html"};
    auto response = cpr::Post(url, Multipart{{"x", 5}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostFileTest) {
    auto filename = std::string{"test_file"};
    auto content = std::string{"hello world"};
    std::ofstream test_file;
    test_file.open(filename);
    test_file << content;
    test_file.close();
    auto url = Url{base + "/form_post.html"};
    auto response = cpr::Post(url, Multipart{{"x", File{filename}}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": " + content + "\n"
                                     "}"};
    std::remove(filename.data());
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostFileNoCopyTest) {
    auto filename = std::string{"test_file"};
    auto content = std::string{"hello world"};
    std::ofstream test_file;
    test_file.open(filename);
    test_file << content;
    test_file.close();
    auto url = Url{base + "/form_post.html"};
    auto multipart = Multipart{{"x", File{filename}}};
    auto response = cpr::Post(url, multipart);
    auto expected_text = std::string{"{\n"
                                     "  \"x\": " + content + "\n"
                                     "}"};
    std::remove(filename.data());
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostFileBufferTest) {
    auto content = std::string{"hello world"};
    auto url = Url{base + "/form_post.html"};
    auto response = cpr::Post(url, Multipart{{"x", Buffer{content.begin(), content.end(), "test_file"}}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": " + content + "\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostFileBufferNoCopyTest) {
    auto content = std::string{"hello world"};
    auto url = Url{base + "/form_post.html"};
    auto multipart = Multipart{{"x", Buffer{content.begin(), content.end(), "test_file"}}};
    auto response = cpr::Post(url, multipart);
    auto expected_text = std::string{"{\n"
                                     "  \"x\": " + content + "\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostFileBufferPointerTest) {
    const char *content = "hello world";
    auto url = Url{base + "/form_post.html"};
    auto response = cpr::Post(url, Multipart{{"x", Buffer{content, 11 + content, "test_file"}}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": " + std::string(content) + "\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostFileBufferArrayTest) {
    const char content[] = "hello world";
    auto url = Url{base + "/form_post.html"};
    // We subtract 1 from std::end() because we don't want to include the terminating null
    auto response = cpr::Post(
            url, Multipart{{"x", Buffer{std::begin(content), std::end(content) - 1, "test_file"}}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": " + std::string(content) + "\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostFileBufferVectorTest) {
    std::vector<unsigned char> content{'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
    auto url = Url{base + "/form_post.html"};
    auto response =
            cpr::Post(url, Multipart{{"x", Buffer{content.begin(), content.end(), "test_file"}}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": hello world\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostFileBufferStdArrayTest) {
    std::array<unsigned char, 11> content{{'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'}};
    auto url = Url{base + "/form_post.html"};
    auto response =
            cpr::Post(url, Multipart{{"x", Buffer{content.begin(), content.end(), "test_file"}}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": hello world\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostManyTest) {
    auto url = Url{base + "/form_post.html"};
    auto response = cpr::Post(url, Multipart{{"x", 5}, {"y", 13}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5,\n"
                                     "  \"y\": 13,\n"
                                     "  \"sum\": 18\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostManyNoCopyTest) {
    auto url = Url{base + "/form_post.html"};
    auto multipart = Multipart{{"x", 5}, {"y", 13}};
    auto response = cpr::Post(url, multipart);
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5,\n"
                                     "  \"y\": 13,\n"
                                     "  \"sum\": 18\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostContentTypeTest) {
    auto url = Url{base + "/form_post.html"};
    auto response = cpr::Post(url, Multipart{{"x", 5, "application/number"}});
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, FormPostContentTypeLValueTest) {
    auto url = Url{base + "/form_post.html"};
    auto multipart = Multipart{{"x", 5, "application/number"}};
    auto response = cpr::Post(url, multipart);
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(UrlEncodedPostTests, UrlPostAsyncSingleTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::PostAsync(url, payload));
    }
    for (auto& future_response : responses) {
        auto response = future_response.get();
        auto expected_text = std::string{"{\n"
                                         "  \"x\": 5\n"
                                         "}"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
        EXPECT_EQ(201, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(UrlEncodedPostTests, UrlReflectTest) {
    auto url = Url{base + "/header_reflect.html"};
    auto response = cpr::Post(url, Payload{{"x", "5"}});
    auto expected_text = std::string{"Header reflect POST"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
