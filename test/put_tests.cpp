#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(PutTests, PutTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    Payload payload{{"x", "5"}};
    Response response = cpr::Put(url, payload);
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

TEST(PutTests, PutUnallowedTest) {
    Url url{server->GetBaseUrl() + "/put_unallowed.html"};
    Payload payload{{"x", "5"}};
    Response response = cpr::Put(url, payload);
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PutTests, SessionPutTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    Payload payload{{"x", "5"}};
    Session session;
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Put();
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

TEST(PutTests, SessionPutUnallowedTest) {
    Url url{server->GetBaseUrl() + "/put_unallowed.html"};
    Payload payload{{"x", "5"}};
    Session session;
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Put();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PutTests, SessionPutAfterGetTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Get();
    }
    Url url{server->GetBaseUrl() + "/put.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Put();
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

TEST(PutTests, SessionPutUnallowedAfterGetTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Get();
    }
    Url url{server->GetBaseUrl() + "/put_unallowed.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Put();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PutTests, SessionPutAfterHeadTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Head();
    }
    Url url{server->GetBaseUrl() + "/put.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Put();
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

TEST(PutTests, SessionPutUnallowedAfterHeadTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Head();
    }
    Url url{server->GetBaseUrl() + "/put_unallowed.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Put();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PutTests, SessionPutAfterPostTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/url_post.html"};
        Payload payload{{"x", "5"}};
        session.SetUrl(url);
        Response response = session.Post();
    }
    Url url{server->GetBaseUrl() + "/put.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Put();
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

TEST(PutTests, SessionPutUnallowedAfterPostTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/url_post.html"};
        Payload payload{{"x", "5"}};
        session.SetUrl(url);
        Response response = session.Post();
    }
    Url url{server->GetBaseUrl() + "/put_unallowed.html"};
    Payload payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    Response response = session.Put();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PutTests, AsyncPutTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    Payload payload{{"x", "5"}};
    cpr::AsyncResponse future_response = cpr::PutAsync(url, payload);
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

TEST(PutTests, AsyncPutUnallowedTest) {
    Url url{server->GetBaseUrl() + "/put_unallowed.html"};
    Payload payload{{"x", "5"}};
    cpr::AsyncResponse future_response = cpr::PutAsync(url, payload);
    cpr::Response response = future_response.get();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PutTests, AsyncMultiplePutTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    Payload payload{{"x", "5"}};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::PutAsync(url, payload));
    }
    for (cpr::AsyncResponse& future_response : responses) {
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

TEST(PutTests, AsyncMultiplePutUnallowedTest) {
    Url url{server->GetBaseUrl() + "/put_unallowed.html"};
    Payload payload{{"x", "5"}};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::PutAsync(url, payload));
    }
    for (cpr::AsyncResponse& future_response : responses) {
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
