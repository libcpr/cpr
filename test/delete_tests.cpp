#include <gtest/gtest.h>

#include <string>

#include <cpr/cpr.h>

#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(DeleteTests, DeleteTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    Response response = cpr::Delete(url);
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, DeleteUnallowedTest) {
    Url url{server->GetBaseUrl() + "/delete_unallowed.html"};
    Response response = cpr::Delete(url);
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, DeleteJsonBodyTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    Response response = cpr::Delete(url, cpr::Body{"'foo': 'bar'"},
                                    cpr::Header{{"Content-Type", "application/json"}});
    std::string expected_text{"'foo': 'bar'"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    Session session;
    session.SetUrl(url);
    Response response = session.Delete();
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteUnallowedTest) {
    Url url{server->GetBaseUrl() + "/delete_unallowed.html"};
    Session session;
    session.SetUrl(url);
    Response response = session.Delete();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteJsonBodyTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{{"Content-Type", "application/json"}});
    session.SetBody(cpr::Body{"{'foo': 'bar'}"});
    Response response = session.Delete();
    std::string expected_text{"{'foo': 'bar'}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteAfterGetTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Get();
    }
    Url url{server->GetBaseUrl() + "/delete.html"};
    session.SetUrl(url);
    Response response = session.Delete();
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteUnallowedAfterGetTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Get();
    }
    Url url{server->GetBaseUrl() + "/delete_unallowed.html"};
    session.SetUrl(url);
    Response response = session.Delete();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteAfterHeadTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Head();
    }
    Url url{server->GetBaseUrl() + "/delete.html"};
    session.SetUrl(url);
    Response response = session.Delete();
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteUnallowedAfterHeadTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/get.html"};
        session.SetUrl(url);
        Response response = session.Head();
    }
    Url url{server->GetBaseUrl() + "/delete_unallowed.html"};
    session.SetUrl(url);
    Response response = session.Delete();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteAfterPostTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/url_post.html"};
        Payload payload{{"x", "5"}};
        session.SetUrl(url);
        Response response = session.Post();
    }
    Url url{server->GetBaseUrl() + "/patch_unallowed.html"};
    session.SetUrl(url);
    Response response = session.Delete();
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, SessionDeleteUnallowedAfterPostTest) {
    Session session;
    {
        Url url{server->GetBaseUrl() + "/url_post.html"};
        Payload payload{{"x", "5"}};
        session.SetUrl(url);
        Response response = session.Post();
    }
    Url url{server->GetBaseUrl() + "/delete_unallowed.html"};
    session.SetUrl(url);
    Response response = session.Delete();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, AsyncDeleteTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    cpr::AsyncResponse future_response = cpr::DeleteAsync(url);
    cpr::Response response = future_response.get();
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, AsyncDeleteUnallowedTest) {
    Url url{server->GetBaseUrl() + "/delete_unallowed.html"};
    cpr::AsyncResponse future_response = cpr::DeleteAsync(url);
    cpr::Response response = future_response.get();
    std::string expected_text{"Method Not Allowed"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/plain"}, response.header["content-type"]);
    EXPECT_EQ(405, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(DeleteTests, AsyncMultipleDeleteTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::DeleteAsync(url));
    }
    for (cpr::AsyncResponse& future_response : responses) {
        cpr::Response response = future_response.get();
        std::string expected_text{"Delete success"};
        EXPECT_EQ(expected_text, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(DeleteTests, AsyncMultipleDeleteUnallowedTest) {
    Url url{server->GetBaseUrl() + "/delete_unallowed.html"};
    std::vector<AsyncResponse> responses;
    for (size_t i = 0; i < 10; ++i) {
        responses.emplace_back(cpr::DeleteAsync(url));
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
