#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "server.h"

using namespace cpr;

static Server* server = new Server();
auto base = server->GetBaseUrl();

TEST(AlternatingTests, PutGetTest) {
    auto url = Url{base + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        auto payload = Payload{{"x", "5"}};
        auto response = cpr::Put(url, payload);
        auto expected_text = std::string{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        auto response = cpr::Get(url);
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(AlternatingTests, PutGetPutGetTest) {
    auto url = Url{base + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        auto payload = Payload{{"x", "5"}};
        auto response = cpr::Put(url, payload);
        auto expected_text = std::string{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        auto response = cpr::Get(url);
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        auto payload = Payload{{"x", "5"}};
        auto response = cpr::Put(url, payload);
        auto expected_text = std::string{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        auto response = cpr::Get(url);
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(AlternatingTests, HeadGetTest) {
    auto url = Url{base + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        // Head shouldn't return a body
        auto response = cpr::Head(url);
        auto expected_text = std::string{""};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        auto response = cpr::Get(url);
        auto expected_text = std::string{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(AlternatingTests, PutHeadTest) {
    auto url = Url{base + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        auto payload = Payload{{"x", "5"}};
        auto response = cpr::Put(url, payload);
        auto expected_text = std::string{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        // Head shouldn't return a body
        auto response = cpr::Head(url);
        auto expected_text = std::string{""};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(AlternatingTests, PutPostTest) {
    auto url = Url{base + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        auto payload = Payload{{"x", "5"}};
        auto response = cpr::Put(url, payload);
        auto expected_text = std::string{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        auto payload = Payload{{"x", "5"}};
        auto response = cpr::Post(url, payload);
        auto expected_text = std::string{"Header reflect POST"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
