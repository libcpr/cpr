#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(PatchTests, PatchTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    Response response = cpr::Patch(url, payload);
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

TEST(PatchTests, PatchUnallowedTest) {
    Url url{server->GetBaseUrl() + "/patch_unallowed.html"};
    Payload payload{{"x", "5"}};
    Response response = cpr::Patch(url, payload);
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    Session session;
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Patch();
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

TEST(PatchTests, SessionPatchUnallowedTest) {
    Url url{server->GetBaseUrl() + "/patch_unallowed.html"};
    Payload payload{{"x", "5"}};
    Session session;
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Patch();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchAfterGetTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Get();
    }
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Patch();
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

TEST(PatchTests, SessionPatchUnallowedAfterGetTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Get();
    }
    Url url{server->GetBaseUrl() + "/patch_unallowed.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Patch();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchAfterHeadTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Head();
    }
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Patch();
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

TEST(PatchTests, SessionPatchUnallowedAfterHeadTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Head();
    }
    Url url{server->GetBaseUrl() + "/patch_unallowed.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Patch();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchAfterPostTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/url_post.html"};
        Payload payload{{"x", "5"}};
        session.SetUrl(url);
        Response response = session.Post();
    }
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Patch();
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

TEST(PatchTests, SessionPatchUnallowedAfterPostTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/url_post.html"};
        Payload payload{{"x", "5"}};
        session.SetUrl(url);
        Response response = session.Post();
    }
    Url url{server->GetBaseUrl() + "/patch_unallowed.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Patch();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, AsyncPatchTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    cpr::AsyncResponse future_response = cpr::PatchAsync(url, payload);
    cpr::Response response = future_response.get();
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

TEST(PatchTests, AsyncPatchUnallowedTest) {
    Url url{server->GetBaseUrl() + "/patch_unallowed.html"};
    Payload payload{{"x", "5"}};
    cpr::AsyncResponse future_response = cpr::PatchAsync(url, payload);
    cpr::Response response = future_response.get();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, AsyncMultiplePatchTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::PatchAsync(url, payload));
    }
    for (auto& future_response : responses) {
        cpr::Response response = future_response.get();
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
}

TEST(PatchTests, AsyncMultiplePatchUnallowedTest) {
    Url url{server->GetBaseUrl() + "/patch_unallowed.html"};
    Payload payload{{"x", "5"}};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::PatchAsync(url, payload));
    }
    for (auto& future_response : responses) {
        cpr::Response response = future_response.get();
        std::string expected_text{"Method Not Allowed"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
        EXPECT_EQ(405, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
