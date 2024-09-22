#include <cstdint>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "cpr/api.h"
#include "cpr/cpr.h"
#include <curl/curl.h>

#include "httpServer.hpp"
#include "httpsServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

bool write_data(const std::string_view& /*data*/, intptr_t /*userdata*/) {
    return true;
}

TEST(MultiperformAddSessionTests, MultiperformAddSingleSessionTest) {
    std::shared_ptr<Session> session = std::make_shared<Session>();
    MultiPerform multiperform;
    multiperform.AddSession(session);

    EXPECT_EQ(2, session.use_count());
}

TEST(MultiperformAddSessionTests, MultiperformAddMultipleSessionsTest) {
    MultiPerform multiperform;
    for (int i = 0; i < 10; ++i) {
        std::shared_ptr<Session> session = std::make_shared<Session>();
        multiperform.AddSession(session);
        EXPECT_EQ(2, session.use_count());
    }
}

TEST(MultiperformAddSessionTests, MultiperformAddMultipleNonDownloadSessionsTest) {
    MultiPerform multiperform;
    for (int i = 0; i < 10; ++i) {
        std::shared_ptr<Session> session = std::make_shared<Session>();
        multiperform.AddSession(session, MultiPerform::HttpMethod::GET_REQUEST);
        EXPECT_EQ(2, session.use_count());
    }
}

TEST(MultiperformAddSessionTests, MultiperformAddMultipleDownloadSessionsTest) {
    MultiPerform multiperform;
    for (int i = 0; i < 10; ++i) {
        std::shared_ptr<Session> session = std::make_shared<Session>();
        multiperform.AddSession(session, MultiPerform::HttpMethod::DOWNLOAD_REQUEST);
        EXPECT_EQ(2, session.use_count());
    }
}

TEST(MultiperformAddSessionTests, MultiperformAddTwoMixedTypeSessionsTest) {
    std::shared_ptr<Session> session_1 = std::make_shared<Session>();
    std::shared_ptr<Session> session_2 = std::make_shared<Session>();
    MultiPerform multiperform;
    multiperform.AddSession(session_1, MultiPerform::HttpMethod::GET_REQUEST);
    EXPECT_EQ(2, session_1.use_count());
    EXPECT_THROW(multiperform.AddSession(session_2, MultiPerform::HttpMethod::DOWNLOAD_REQUEST), std::invalid_argument);
}

TEST(MultiperformAddSessionTests, MultiperformAddTwoMixedTypeSessionsReversTest) {
    std::shared_ptr<Session> session_1 = std::make_shared<Session>();
    std::shared_ptr<Session> session_2 = std::make_shared<Session>();
    MultiPerform multiperform;
    multiperform.AddSession(session_1, MultiPerform::HttpMethod::DOWNLOAD_REQUEST);
    EXPECT_EQ(2, session_1.use_count());
    EXPECT_THROW(multiperform.AddSession(session_2, MultiPerform::HttpMethod::GET_REQUEST), std::invalid_argument);
}

TEST(MultiperformRemoveSessionTests, MultiperformRemoveSingleSessionTest) {
    std::shared_ptr<Session> session = std::make_shared<Session>();
    MultiPerform multiperform;
    multiperform.AddSession(session);
    EXPECT_EQ(2, session.use_count());
    multiperform.RemoveSession(session);
    EXPECT_EQ(1, session.use_count());
}

TEST(MultiperformRemoveSessionTests, MultiperformRemoveMultipleSessionsTest) {
    MultiPerform multiperform;
    for (int i = 0; i < 10; ++i) {
        std::shared_ptr<Session> session = std::make_shared<Session>();
        multiperform.AddSession(session);
        EXPECT_EQ(2, session.use_count());
        multiperform.RemoveSession(session);
        EXPECT_EQ(1, session.use_count());
    }
}

TEST(MultiperformRemoveSessionTests, MultiperformRemoveNonExistingSessionEmptyTest) {
    std::shared_ptr<Session> session = std::make_shared<Session>();
    MultiPerform multiperform;
    EXPECT_THROW(multiperform.RemoveSession(session), std::invalid_argument);
}

TEST(MultiperformRemoveSessionTests, MultiperformRemoveNonExistingSessionTest) {
    MultiPerform multiperform;
    std::shared_ptr<Session> session = std::make_shared<Session>();
    multiperform.AddSession(session);

    std::shared_ptr<Session> session2 = std::make_shared<Session>();
    EXPECT_THROW(multiperform.RemoveSession(session2), std::invalid_argument);
}

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
    expected_texts.emplace_back("Hello world!");
    expected_texts.emplace_back("Not Found");

    std::vector<std::string> expected_content_types;
    expected_content_types.emplace_back("text/html");
    expected_content_types.emplace_back("text/plain");

    std::vector<uint16_t> expected_status_codes;
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

#ifndef __APPLE__
/**
 * This test case is currently disabled for macOS/Apple systems since it fails in an nondeterministic manner.
 * It is probably caused by a bug inside curl_multi_perform on macOS.
 * Needs further investigation.
 * Issue: https://github.com/libcpr/cpr/issues/841
 **/
TEST(MultiperformGetTests, MultiperformTenSessionsGetTest) {
    const size_t sessionCount = 10;

    MultiPerform multiperform;
    Url url{server->GetBaseUrl() + "/hello.html"};
    for (size_t i = 0; i < sessionCount; ++i) {
        std::shared_ptr<Session> session = std::make_shared<Session>();
        session->SetUrl(url);
        multiperform.AddSession(session);
    }

    std::vector<Response> responses = multiperform.Get();

    EXPECT_EQ(responses.size(), sessionCount);
    for (Response& response : responses) {
        EXPECT_EQ(std::string{"Hello world!"}, response.text);
        EXPECT_EQ(url, response.url);
        EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
        EXPECT_EQ(200, response.status_code);
        EXPECT_EQ(ErrorCode::OK, response.error.code);
    }
}
#endif

TEST(MultiperformDeleteTests, MultiperformSingleSessionDeleteTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multiperform;
    multiperform.AddSession(session);
    std::vector<Response> responses = multiperform.Delete();

    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformDownloadTests, MultiperformSingleSessionDownloadTest) {
    Url url{server->GetBaseUrl() + "/download_gzip.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    MultiPerform multiperform;
    multiperform.AddSession(session);
    std::vector<Response> responses = multiperform.Download(WriteCallback{write_data, 0});

    EXPECT_EQ(responses.size(), 1);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformDownloadTests, MultiperformSingleSessionDownloadNonMatchingArgumentsNumberTest) {
    std::shared_ptr<Session> session = std::make_shared<Session>();
    MultiPerform multiperform;
    multiperform.AddSession(session);
    EXPECT_THROW(std::vector<Response> responses = multiperform.Download(WriteCallback{write_data, 0}, WriteCallback{write_data, 0}), std::invalid_argument);
}

TEST(MultiperformDownloadTests, MultiperformTwoSessionsDownloadTest) {
    MultiPerform multiperform;
    std::vector<Url> urls;
    urls.push_back({server->GetBaseUrl() + "/download_gzip.html"});
    urls.push_back({server->GetBaseUrl() + "/download_gzip.html"});

    std::vector<std::shared_ptr<Session>> sessions;
    sessions.push_back(std::make_shared<Session>());
    sessions.push_back(std::make_shared<Session>());


    for (size_t i = 0; i < sessions.size(); ++i) {
        sessions.at(i)->SetUrl(urls.at(i));
        sessions.at(i)->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});

        multiperform.AddSession(sessions.at(i));
    }

    std::vector<Response> responses = multiperform.Download(WriteCallback{write_data, 0}, WriteCallback{write_data, 0});

    EXPECT_EQ(responses.size(), sessions.size());
    for (size_t i = 0; i < responses.size(); ++i) {
        EXPECT_EQ(urls.at(i), responses.at(i).url);
        EXPECT_EQ(200, responses.at(i).status_code);
        EXPECT_EQ(ErrorCode::OK, responses.at(i).error.code);
    }
}

TEST(MultiperformPutTests, MultiperformSingleSessionPutTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetPayload(Payload{{"x", "5"}});
    MultiPerform multiperform;
    multiperform.AddSession(session);
    std::vector<Response> responses = multiperform.Put();

    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(std::string{"application/json"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformHeadTests, MultiperformSingleSessionHeadTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multiperform;
    multiperform.AddSession(session);
    std::vector<Response> responses = multiperform.Head();

    EXPECT_EQ(responses.size(), 1);
    std::string expected_text;
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformOptionsTests, MultiperformSingleSessionOptionsTest) {
    Url url{server->GetBaseUrl() + "/"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multiperform;
    multiperform.AddSession(session);
    std::vector<Response> responses = multiperform.Options();

    EXPECT_EQ(responses.size(), 1);
    std::string expected_text;
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, responses.at(0).header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformPatchTests, MultiperformSingleSessionPatchTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetPayload(Payload{{"x", "5"}});
    MultiPerform multiperform;
    multiperform.AddSession(session);
    std::vector<Response> responses = multiperform.Patch();

    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(std::string{"application/json"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformPostTests, MultiperformSingleSessionPostTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetPayload(Payload{{"x", "5"}});
    MultiPerform multiperform;
    multiperform.AddSession(session);
    std::vector<Response> responses = multiperform.Post();

    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(std::string{"application/json"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(201, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformPerformTests, MultiperformSingleGetPerformTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multiperform;
    multiperform.AddSession(session, MultiPerform::HttpMethod::GET_REQUEST);
    std::vector<Response> responses = multiperform.Perform();

    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformPerformTests, MultiperformTwoGetPerformTest) {
    MultiPerform multiperform;
    std::vector<Url> urls;
    urls.push_back({server->GetBaseUrl() + "/hello.html"});
    urls.push_back({server->GetBaseUrl() + "/error.html"});

    std::vector<std::shared_ptr<Session>> sessions;
    sessions.push_back(std::make_shared<Session>());
    sessions.push_back(std::make_shared<Session>());


    for (size_t i = 0; i < sessions.size(); ++i) {
        sessions.at(i)->SetUrl(urls.at(i));
        multiperform.AddSession(sessions.at(i), MultiPerform::HttpMethod::GET_REQUEST);
    }

    std::vector<Response> responses = multiperform.Perform();

    EXPECT_EQ(responses.size(), sessions.size());
    std::vector<std::string> expected_texts;
    expected_texts.emplace_back("Hello world!");
    expected_texts.emplace_back("Not Found");

    std::vector<std::string> expected_content_types;
    expected_content_types.emplace_back("text/html");
    expected_content_types.emplace_back("text/plain");

    std::vector<uint16_t> expected_status_codes;
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

TEST(MultiperformPerformTests, MultiperformMixedMethodsPerformTest) {
    MultiPerform multiperform;
    std::vector<Url> urls;
    urls.push_back({server->GetBaseUrl() + "/hello.html"});
    urls.push_back({server->GetBaseUrl() + "/delete.html"});
    urls.push_back({server->GetBaseUrl() + "/error.html"});
    urls.push_back({server->GetBaseUrl() + "/url_post.html"});

    std::vector<std::shared_ptr<Session>> sessions;
    sessions.push_back(std::make_shared<Session>());
    sessions.push_back(std::make_shared<Session>());
    sessions.push_back(std::make_shared<Session>());
    sessions.push_back(std::make_shared<Session>());

    std::vector<MultiPerform::HttpMethod> methods;
    methods.push_back(MultiPerform::HttpMethod::GET_REQUEST);
    methods.push_back(MultiPerform::HttpMethod::DELETE_REQUEST);
    methods.push_back(MultiPerform::HttpMethod::GET_REQUEST);
    methods.push_back(MultiPerform::HttpMethod::POST_REQUEST);

    for (size_t i = 0; i < sessions.size(); ++i) {
        sessions.at(i)->SetUrl(urls.at(i));
        if (methods.at(i) == MultiPerform::HttpMethod::POST_REQUEST) {
            sessions.at(i)->SetPayload(Payload{{"x", "5"}});
        }
        multiperform.AddSession(sessions.at(i), methods.at(i));
    }

    std::vector<Response> responses = multiperform.Perform();

    EXPECT_EQ(responses.size(), sessions.size());

    std::vector<std::string> expected_texts;
    expected_texts.emplace_back("Hello world!");
    expected_texts.emplace_back("Delete success");
    expected_texts.emplace_back("Not Found");
    expected_texts.emplace_back(
            "{\n"
            "  \"x\": 5\n"
            "}");

    std::vector<std::string> expected_content_types;
    expected_content_types.emplace_back("text/html");
    expected_content_types.emplace_back("text/html");
    expected_content_types.emplace_back("text/plain");
    expected_content_types.emplace_back("application/json");

    std::vector<uint16_t> expected_status_codes;
    expected_status_codes.push_back(200);
    expected_status_codes.push_back(200);
    expected_status_codes.push_back(404);
    expected_status_codes.push_back(201);

    for (size_t i = 0; i < responses.size(); ++i) {
        EXPECT_EQ(expected_texts.at(i), responses.at(i).text);
        EXPECT_EQ(urls.at(i), responses.at(i).url);
        EXPECT_EQ(expected_content_types.at(i), responses.at(i).header["content-type"]);
        EXPECT_EQ(expected_status_codes.at(i), responses.at(i).status_code);
        EXPECT_EQ(ErrorCode::OK, responses.at(i).error.code);
    }
}

TEST(MultiperformPerformDownloadTests, MultiperformSinglePerformDownloadTest) {
    Url url{server->GetBaseUrl() + "/download_gzip.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    MultiPerform multiperform;
    multiperform.AddSession(session, MultiPerform::HttpMethod::DOWNLOAD_REQUEST);
    std::vector<Response> responses = multiperform.PerformDownload(WriteCallback{write_data, 0});

    EXPECT_EQ(responses.size(), 1);
    EXPECT_EQ(url, responses.at(0).url);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformDownloadTests, MultiperformSinglePerformDownloadNonMatchingArgumentsNumberTest) {
    std::shared_ptr<Session> session = std::make_shared<Session>();
    MultiPerform multiperform;
    multiperform.AddSession(session, MultiPerform::HttpMethod::DOWNLOAD_REQUEST);
    EXPECT_THROW(std::vector<Response> responses = multiperform.PerformDownload(WriteCallback{write_data, 0}, WriteCallback{write_data, 0}), std::invalid_argument);
}

TEST(MultiperformPerformDownloadTests, MultiperformTwoPerformDownloadTest) {
    MultiPerform multiperform;
    std::vector<Url> urls;
    urls.push_back({server->GetBaseUrl() + "/download_gzip.html"});
    urls.push_back({server->GetBaseUrl() + "/download_gzip.html"});

    std::vector<std::shared_ptr<Session>> sessions;
    sessions.push_back(std::make_shared<Session>());
    sessions.push_back(std::make_shared<Session>());


    for (size_t i = 0; i < sessions.size(); ++i) {
        sessions.at(i)->SetUrl(urls.at(i));
        sessions.at(i)->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});

        multiperform.AddSession(sessions.at(i), MultiPerform::HttpMethod::DOWNLOAD_REQUEST);
    }

    std::vector<Response> responses = multiperform.PerformDownload(WriteCallback{write_data, 0}, WriteCallback{write_data, 0});

    EXPECT_EQ(responses.size(), sessions.size());
    for (size_t i = 0; i < responses.size(); ++i) {
        EXPECT_EQ(urls.at(i), responses.at(i).url);
        EXPECT_EQ(200, responses.at(i).status_code);
        EXPECT_EQ(ErrorCode::OK, responses.at(i).error.code);
    }
}

TEST(MultiperformAPITests, MultiperformApiSingleGetTest) {
    std::vector<Response> responses = MultiGet(std::tuple<Url>{Url{server->GetBaseUrl() + "/hello.html"}});
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/hello.html"}, responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformAPITests, MultiperformApiTwoGetsTest) {
    std::vector<Response> responses = MultiGet(std::tuple<Url, Timeout>{Url{server->GetBaseUrl() + "/long_timeout.html"}, Timeout{1000}}, std::tuple<Url>{Url{server->GetBaseUrl() + "/error.html"}});

    EXPECT_EQ(responses.size(), 2);
    std::vector<Url> urls;
    urls.push_back({server->GetBaseUrl() + "/long_timeout.html"});
    urls.push_back({server->GetBaseUrl() + "/error.html"});

    std::vector<std::string> expected_texts;
    expected_texts.emplace_back("");
    expected_texts.emplace_back("Not Found");

    std::vector<std::string> expected_content_types;
    expected_content_types.emplace_back("");
    expected_content_types.emplace_back("text/plain");

    std::vector<uint16_t> expected_status_codes;
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

TEST(MultiperformAPITests, MultiperformApiSingleDeleteTest) {
    std::vector<Response> responses = MultiDelete(std::tuple<Url>{Url{server->GetBaseUrl() + "/delete.html"}});
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/delete.html"}, responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformAPITests, MultiperformApiSinglePutTest) {
    std::vector<Response> responses = MultiPut(std::tuple<Url, Payload>{Url{server->GetBaseUrl() + "/put.html"}, Payload{{"x", "5"}}});
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/put.html"}, responses.at(0).url);
    EXPECT_EQ(std::string{"application/json"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformAPITests, MultiperformApiSingleHeadTest) {
    std::vector<Response> responses = MultiHead(std::tuple<Url>{Url{server->GetBaseUrl() + "/hello.html"}});
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text;
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/hello.html"}, responses.at(0).url);
    EXPECT_EQ(std::string{"text/html"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformAPITests, MultiperformApiSingleOptionsTest) {
    std::vector<Response> responses = MultiOptions(std::tuple<Url>{Url{server->GetBaseUrl() + "/"}});
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text;
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/"}, responses.at(0).url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, responses.at(0).header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformAPITests, MultiperformApiSinglePatchTest) {
    std::vector<Response> responses = MultiPatch(std::tuple<Url, Payload>{Url{server->GetBaseUrl() + "/patch.html"}, Payload{{"x", "5"}}});
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/patch.html"}, responses.at(0).url);
    EXPECT_EQ(std::string{"application/json"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(200, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

TEST(MultiperformAPITests, MultiperformApiSinglePostTest) {
    std::vector<Response> responses = MultiPost(std::tuple<Url, Payload>{Url{server->GetBaseUrl() + "/url_post.html"}, Payload{{"x", "5"}}});
    EXPECT_EQ(responses.size(), 1);
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, responses.at(0).text);
    EXPECT_EQ(Url{server->GetBaseUrl() + "/url_post.html"}, responses.at(0).url);
    EXPECT_EQ(std::string{"application/json"}, responses.at(0).header["content-type"]);
    EXPECT_EQ(201, responses.at(0).status_code);
    EXPECT_EQ(ErrorCode::OK, responses.at(0).error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
