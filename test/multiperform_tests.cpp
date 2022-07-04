#include <gtest/gtest.h>

#include <string>

#include <cpr/api.h>
#include <cpr/cpr.h>
#include <curl/curl.h>

#include "httpServer.hpp"
#include "httpsServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(MultiperformGetTests, MultiperformSingleSessionGetTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multiperform;
    multiperform.AddSession(session);
    std::vector<Response> responses = multiperform.Get();

    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformGetTests, MultiperformTwoSessionsGetTest) {
    MultiPerform multiperform;
    std::vector<Url> urls;
    urls.push_back({server->GetBaseUrl() + "/hello.html"});
    urls.push_back({server->GetBaseUrl() + "/error.html"});

    std::vector<std::shared_ptr<Session>> sessions;
    sessions.push_back(std::make_shared<Session>());
    sessions.push_back(std::make_shared<Session>());


    for (size_t i = 0; i < sessions.size(); ++i) {
        sessions.at(i)->SetUrl(urls.at(i));
        multiperform.AddSession(sessions.at(i));
    }

    std::vector<Response> responses = multiperform.Get();

    EXPECT_EQ(responses.size(), sessions.size());
    std::vector<std::string> expected_texts;
    expected_texts.push_back("Hello world!");
    expected_texts.push_back("Not Found");

    std::vector<std::string> expected_content_types;
    expected_content_types.push_back("text/html");
    expected_content_types.push_back("text/plain");

    std::vector<long> expected_status_codes;
    expected_status_codes.push_back(200);
    expected_status_codes.push_back(404);

    for (size_t i = 0; i < responses.size(); ++i) {
        EXPECT_EQ(expected_texts.at(i), responses.at(i).text);
        EXPECT_EQ(urls.at(i), responses.at(i).url);
        EXPECT_EQ(expected_content_types.at(i), responses.at(i).header["content-type"]);
        EXPECT_EQ(expected_status_codes.at(i), responses.at(i).status_code);
        EXPECT_EQ(ErrorCode::OK, responses.at(i).error.code);
    }
}

TEST(MultiperformGetTests, MultiperformRemoveSessionGetTest) {
    MultiPerform multiperform;
    std::vector<Url> urls;
    urls.push_back({server->GetBaseUrl() + "/hello.html"});
    urls.push_back({server->GetBaseUrl() + "/hello.html"});

    std::vector<std::shared_ptr<Session>> sessions;
    sessions.push_back(std::make_shared<Session>());
    sessions.push_back(std::make_shared<Session>());


    for (size_t i = 0; i < sessions.size(); ++i) {
        sessions.at(i)->SetUrl(urls.at(i));
        multiperform.AddSession(sessions.at(i));
    }

    multiperform.RemoveSession(sessions.at(0));

    std::vector<Response> responses = multiperform.Get();
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(urls.at(0), responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformGetTests, MultiperformTenSessionsGetTest) {
    MultiPerform multiperform;
    std::vector<Url> urls;
    std::vector<std::shared_ptr<Session>> sessions;
    Url url{server->GetBaseUrl() + "/hello.html"};
    for (size_t i = 0; i < 10; ++i) {
        urls.push_back(url);
        sessions.push_back(std::make_shared<Session>());
        sessions.at(i)->SetUrl(urls.at(i));
        multiperform.AddSession(sessions.at(i));
    }

    std::vector<Response> responses = multiperform.Get();

    EXPECT_EQ(responses.size(), 10);
    for (Response& response : responses) {
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}

TEST(MultiperformGetTests, MultiperformApiSingleGetTest) {
    std::vector<Response> responses = MultiGet(std::tuple<Url>{Url{server->GetBaseUrl() + "/hello.html"}});
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/hello.html"}, responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformGetTests, MultiperformApiTwoGetsTest) {
    std::vector<Response> responses = MultiGet(std::tuple<Url, Timeout>{Url{server->GetBaseUrl() + "/low_speed_timeout.html"}, Timeout{1000}}, std::tuple<Url>{Url{server->GetBaseUrl() + "/error.html"}});

    EXPECT_EQ(responses.size(), 2);
    std::vector<Url> urls;
    urls.push_back({server->GetBaseUrl() + "/low_speed_timeout.html"});
    urls.push_back({server->GetBaseUrl() + "/error.html"});

    std::vector<std::string> expected_texts;
    expected_texts.push_back("");
    expected_texts.push_back("Not Found");

    std::vector<std::string> expected_content_types;
    expected_content_types.push_back("");
    expected_content_types.push_back("text/plain");

    std::vector<long> expected_status_codes;
    expected_status_codes.push_back(0);
    expected_status_codes.push_back(404);

    std::vector<ErrorCode> expected_error_codes;
    expected_error_codes.push_back(ErrorCode::OPERATION_TIMEDOUT);
    expected_error_codes.push_back(ErrorCode::OK);

    for (size_t i = 0; i < responses.size(); ++i) {
        EXPECT_EQ(expected_texts.at(i), responses.at(i).text);
        EXPECT_EQ(urls.at(i), responses.at(i).url);
        EXPECT_EQ(expected_content_types.at(i), responses.at(i).header["content-type"]);
        EXPECT_EQ(expected_status_codes.at(i), responses.at(i).status_code);
        EXPECT_EQ(expected_error_codes.at(i), responses.at(i).error.code);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}