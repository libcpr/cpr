#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(AlternatingTests, PutGetTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        Payload payload{{"x", "5"}};
        Response response = cpr::Put(url, payload);
        std::string expected_text{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        Response response = cpr::Get(url);
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(AlternatingTests, PutGetPutGetTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        Payload payload{{"x", "5"}};
        Response response = cpr::Put(url, payload);
        std::string expected_text{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        Response response = cpr::Get(url);
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        Payload payload{{"x", "5"}};
        Response response = cpr::Put(url, payload);
        std::string expected_text{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        Response response = cpr::Get(url);
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(AlternatingTests, HeadGetTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        // Head shouldn't return a body
        Response response = cpr::Head(url);
        std::string expected_text{""};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        Response response = cpr::Get(url);
        std::string expected_text{"Header reflect GET"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(AlternatingTests, PutHeadTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        Payload payload{{"x", "5"}};
        Response response = cpr::Put(url, payload);
        std::string expected_text{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        // Head shouldn't return a body
        Response response = cpr::Head(url);
        std::string expected_text{""};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(AlternatingTests, PutPostTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    Session session;
    session.SetUrl(url);

    {
        Payload payload{{"x", "5"}};
        Response response = cpr::Put(url, payload);
        std::string expected_text{"Header reflect PUT"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }

    {
        Payload payload{{"x", "5"}};
        Response response = cpr::Post(url, payload);
        std::string expected_text{"Header reflect POST"};
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
