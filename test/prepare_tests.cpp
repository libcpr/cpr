#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(PrepareTests, GetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.PrepareGet();
    CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
    Response response = session.Complete(curl_result);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PrepareTests, OptionsTests) {
    Url url{server->GetBaseUrl() + "/"};
    Session session;
    session.SetUrl(url);
    session.PrepareOptions();
    CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
    Response response = session.Complete(curl_result);
    std::string expected_text{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"},
              response.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PrepareTests, PatchTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    Session session;
    session.SetUrl(url);
    session.SetPayload(payload);
    session.PreparePatch();
    CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
    Response response = session.Complete(curl_result);
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PrepareTests, MultipleDeleteHeadPutGetPostTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Url urlPost{server->GetBaseUrl() + "/reflect_post.html"};
    Url urlPut{server->GetBaseUrl() + "/put.html"};
    Session session;
    for (size_t i = 0; i < 3; ++i) {
        {
            session.SetUrl(url);
            session.PrepareDelete();
            CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
            Response response = session.Complete(curl_result);
            std::string expected_text{"Header reflect DELETE"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            session.SetUrl(urlPost);
            std::string expectedBody = "a1b2c3Post";
            session.SetBody(expectedBody);
            session.PreparePost();
            CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
            Response response = session.Complete(curl_result);
            EXPECT_EQ(expectedBody, response.text);
            EXPECT_EQ(urlPost, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            session.SetUrl(url);
            session.PrepareGet();
            CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
            Response response = session.Complete(curl_result);
            std::string expected_text{"Header reflect GET"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            session.SetUrl(urlPut);
            session.SetPayload({{"x", "5"}});
            session.PreparePut();
            CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
            Response response = session.Complete(curl_result);
            std::string expected_text{
                    "{\n"
                    "  \"x\": 5\n"
                    "}"};
            EXPECT_EQ(expected_text, response.text);
            EXPECT_EQ(urlPut, response.url);
            EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
        {
            session.SetUrl(url);
            session.PrepareHead();
            CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
            Response response = session.Complete(curl_result);
            std::string expected_text{"Header reflect HEAD"};
            EXPECT_EQ(url, response.url);
            EXPECT_EQ(200, response.status_code);
            EXPECT_EQ(ErrorCode::OK, response.error.code);
        }
    }
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
