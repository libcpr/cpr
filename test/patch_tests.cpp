#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "server.h"

using namespace cpr;

static Server* server = new Server();
auto base = server->GetBaseUrl();

TEST(PatchTests, PatchTest) {
    auto url = Url{base + "/patch.html"};
    auto payload = Payload{{"x", "5"}};
    auto response = cpr::Patch(url, payload);
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, PatchUnallowedTest) {
    auto url = Url{base + "/patch_unallowed.html"};
    auto payload = Payload{{"x", "5"}};
    auto response = cpr::Patch(url, payload);
    auto expected_text = std::string{"Method unallowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchTest) {
    auto url = Url{base + "/patch.html"};
    auto payload = Payload{{"x", "5"}};
    Session session;
    session.SetUrl(url);
    session.SetPayload(payload);
    auto response = session.Patch();
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchUnallowedTest) {
    auto url = Url{base + "/patch_unallowed.html"};
    auto payload = Payload{{"x", "5"}};
    Session session;
    session.SetUrl(url);
    session.SetPayload(payload);
    auto response = session.Patch();
    auto expected_text = std::string{"Method unallowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchAfterGetTest) {
    Session session;
    {
        auto url = Url{base + "/get.html"};
        session.SetUrl(url);
        auto response = session.Get();
    }
    auto url = Url{base + "/patch.html"};
    auto payload = Payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    auto response = session.Patch();
    auto expected_text = std::string{"{\n"
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
        auto url = Url{base + "/get.html"};
        session.SetUrl(url);
        auto response = session.Get();
    }
    auto url = Url{base + "/patch_unallowed.html"};
    auto payload = Payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    auto response = session.Patch();
    auto expected_text = std::string{"Method unallowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchAfterHeadTest) {
    Session session;
    {
        auto url = Url{base + "/get.html"};
        session.SetUrl(url);
        auto response = session.Head();
    }
    auto url = Url{base + "/patch.html"};
    auto payload = Payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    auto response = session.Patch();
    auto expected_text = std::string{"{\n"
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
        auto url = Url{base + "/get.html"};
        session.SetUrl(url);
        auto response = session.Head();
    }
    auto url = Url{base + "/patch_unallowed.html"};
    auto payload = Payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    auto response = session.Patch();
    auto expected_text = std::string{"Method unallowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, SessionPatchAfterPostTest) {
    Session session;
    {
        auto url = Url{base + "/url_post.html"};
        auto payload = Payload{{"x", "5"}};
        session.SetUrl(url);
        auto response = session.Post();
    }
    auto url = Url{base + "/patch.html"};
    auto payload = Payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    auto response = session.Patch();
    auto expected_text = std::string{"{\n"
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
        auto url = Url{base + "/url_post.html"};
        auto payload = Payload{{"x", "5"}};
        session.SetUrl(url);
        auto response = session.Post();
    }
    auto url = Url{base + "/patch_unallowed.html"};
    auto payload = Payload{{"x", "5"}};
    session.SetUrl(url);
    session.SetPayload(payload);
    auto response = session.Patch();
    auto expected_text = std::string{"Method unallowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, AsyncPatchTest) {
    auto url = Url{base + "/patch.html"};
    auto payload = Payload{{"x", "5"}};
    auto future_response = cpr::PatchAsync(url, payload);
    auto response = future_response.get();
    auto expected_text = std::string{"{\n"
                                     "  \"x\": 5\n"
                                     "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, AsyncPatchUnallowedTest) {
    auto url = Url{base + "/patch_unallowed.html"};
    auto payload = Payload{{"x", "5"}};
    auto future_response = cpr::PatchAsync(url, payload);
    auto response = future_response.get();
    auto expected_text = std::string{"Method unallowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(PatchTests, AsyncMultiplePatchTest) {
    auto url = Url{base + "/patch.html"};
    auto payload = Payload{{"x", "5"}};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::PatchAsync(url, payload));
    }
    for (auto& future_response : responses) {
        auto response = future_response.get();
        auto expected_text = std::string{"{\n"
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
    auto url = Url{base + "/patch_unallowed.html"};
    auto payload = Payload{{"x", "5"}};
    std::vector<AsyncResponse> responses;
    for (int i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::PatchAsync(url, payload));
    }
    for (auto& future_response : responses) {
        auto response = future_response.get();
        auto expected_text = std::string{"Method unallowed"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(405, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
