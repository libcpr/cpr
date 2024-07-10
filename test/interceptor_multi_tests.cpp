#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "cpr/cpr.h"
#include "cpr/interceptor.h"
#include "cpr/response.h"
#include "cpr/session.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

class HiddenHelloWorldRedirectInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        std::shared_ptr<Session> session = multi.GetSessions().front().first;

        // Save original url
        Url old_url = session->GetFullRequestUrl();

        // Rewrite the url
        Url url{server->GetBaseUrl() + "/hello.html"};
        session->SetUrl(url);

        // Proceed the chain
        std::vector<Response> response = proceed(multi);
        EXPECT_FALSE(response.empty());

        // Restore the url again
        response.front().url = old_url;
        return response;
    }
};

class ChangeStatusCodeInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        std::shared_ptr<Session> session = multi.GetSessions().front().first;

        // Proceed the chain
        std::vector<Response> response = proceed(multi);
        EXPECT_FALSE(response.empty());

        // Change the status code
        response.front().status_code = 12345;
        return response;
    }
};

class SetBasicAuthInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        std::shared_ptr<Session> session = multi.GetSessions().front().first;

        // Set authentication
        session->SetAuth(Authentication{"user", "password", AuthMode::BASIC});

        // Proceed the chain
        return proceed(multi);
    }
};

class SetUnsupportedProtocolErrorInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        std::shared_ptr<Session> session = multi.GetSessions().front().first;

        // Proceed the chain
        std::vector<Response> response = proceed(multi);
        EXPECT_FALSE(response.empty());

        // Set unsupported protocol error
        response.front().error = Error{CURLE_UNSUPPORTED_PROTOCOL, "SetErrorInterceptorMulti"};

        // Return response
        return response;
    }
};

class ChangeRequestMethodToGetInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        multi.GetSessions().front().second = MultiPerform::HttpMethod::GET_REQUEST;

        return proceed(multi);
    }
};

class ChangeRequestMethodToPostInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        multi.GetSessions().front().second = MultiPerform::HttpMethod::POST_REQUEST;
        multi.GetSessions().front().first->SetOption(Payload{{"x", "5"}});
        return proceed(multi);
    }
};

class ChangeRequestMethodToPutInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        multi.GetSessions().front().second = MultiPerform::HttpMethod::PUT_REQUEST;
        multi.GetSessions().front().first->SetOption(Payload{{"x", "5"}});
        return proceed(multi);
    }
};

class ChangeRequestMethodToDeleteInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        multi.GetSessions().front().second = MultiPerform::HttpMethod::DELETE_REQUEST;
        return proceed(multi);
    }
};

bool write_data(const std::string_view& /*data*/, intptr_t /*userdata*/) {
    return true;
}

class ChangeRequestMethodToDownloadCallbackInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        multi.GetSessions().front().second = MultiPerform::HttpMethod::DOWNLOAD_REQUEST;
        PrepareDownloadSession(multi, 0, WriteCallback{write_data, 0});
        return proceed(multi);
    }
};

class ChangeRequestMethodToHeadInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        multi.GetSessions().front().second = MultiPerform::HttpMethod::HEAD_REQUEST;
        return proceed(multi);
    }
};

class ChangeRequestMethodToOptionsInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        multi.GetSessions().front().second = MultiPerform::HttpMethod::OPTIONS_REQUEST;
        return proceed(multi);
    }
};

class ChangeRequestMethodToPatchInterceptorMulti : public InterceptorMulti {
  public:
    std::vector<Response> intercept(MultiPerform& multi) override {
        EXPECT_FALSE(multi.GetSessions().empty());
        multi.GetSessions().front().second = MultiPerform::HttpMethod::PATCH_REQUEST;
        multi.GetSessions().front().first->SetOption(Payload{{"x", "5"}});
        return proceed(multi);
    }
};

TEST(InterceptorMultiTest, HiddenUrlRewriteInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<HiddenHelloWorldRedirectInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeStatusCodeInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeStatusCodeInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    long expected_status_code{12345};
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(expected_status_code, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, DownloadChangeStatusCodeInterceptorMultiTest) {
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeStatusCodeInterceptorMulti>());
    std::vector<Response> response = multi.Download(cpr::WriteCallback{write_data, 0});
    EXPECT_EQ(response.size(), 1);

    long expected_status_code{12345};
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(expected_status_code, response.front().status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, SetBasicAuthInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<SetBasicAuthInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"text/html"}, response.front().header["content-type"]);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, SetUnsupportedProtocolErrorInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<SetUnsupportedProtocolErrorInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_error_message{"SetErrorInterceptorMulti"};
    ErrorCode expected_error_code{ErrorCode::UNSUPPORTED_PROTOCOL};
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"text/html"}, response.front().header["content-type"]);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(expected_error_message, response.front().error.message);
    EXPECT_EQ(expected_error_code, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToGetInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToGetInterceptorMulti>());
    std::vector<Response> response = multi.Head();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"text/html"}, response.front().header["content-type"]);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToPostInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToPostInterceptorMulti>());
    std::vector<Response> response = multi.Head();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"application/json"}, response.front().header["content-type"]);
    EXPECT_EQ(201, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToPutInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToPutInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"application/json"}, response.front().header["content-type"]);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToPatchInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToPatchInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"application/json"}, response.front().header["content-type"]);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToOptionsInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToOptionsInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{""};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, response.front().header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToHeadInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToHeadInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    EXPECT_EQ(std::string{}, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"text/html"}, response.front().header["content-type"]);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToDownloadCallbackInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/download_gzip.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    session->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session->SetTimeout(Timeout{2000});
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToDownloadCallbackInterceptorMulti>());
    std::vector<Response> response = multi.Put();
    EXPECT_EQ(response.size(), 1);

    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToDownloadCallbackInterceptorMultiMixTest) {
    Url url{server->GetBaseUrl() + "/download_gzip.html"};
    std::shared_ptr<Session> session1 = std::make_shared<Session>();
    session1->SetUrl(url);
    session1->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session1->SetTimeout(Timeout{2000});

    std::shared_ptr<Session> session2 = std::make_shared<Session>();
    session2->SetUrl(url);
    session2->SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session2->SetTimeout(Timeout{2000});

    MultiPerform multi;
    multi.AddSession(session1);
    multi.AddSession(session2);
    // Changes only one of two sessions to download, so it is expected to throw an exception here since we can not mix them.
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToDownloadCallbackInterceptorMulti>());
    EXPECT_THROW(multi.Put(), std::invalid_argument);
}

TEST(InterceptorMultiTest, ChangeRequestMethodToDeleteInterceptorMultiTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<ChangeRequestMethodToDeleteInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(std::string{"text/html"}, response.front().header["content-type"]);
    EXPECT_EQ(200, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, TwoInterceptorMultisTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<HiddenHelloWorldRedirectInterceptorMulti>());
    multi.AddInterceptor(std::make_shared<ChangeStatusCodeInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{"Hello world!"};
    long expected_status_code{12345};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(expected_status_code, response.front().status_code);
    EXPECT_EQ(ErrorCode::OK, response.front().error.code);
}

TEST(InterceptorMultiTest, ThreeInterceptorMultisTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    std::shared_ptr<Session> session = std::make_shared<Session>();
    session->SetUrl(url);
    MultiPerform multi;
    multi.AddSession(session);
    multi.AddInterceptor(std::make_shared<HiddenHelloWorldRedirectInterceptorMulti>());
    multi.AddInterceptor(std::make_shared<ChangeStatusCodeInterceptorMulti>());
    multi.AddInterceptor(std::make_shared<SetUnsupportedProtocolErrorInterceptorMulti>());
    std::vector<Response> response = multi.Get();
    EXPECT_EQ(response.size(), 1);

    std::string expected_text{"Hello world!"};
    long expected_status_code{12345};
    std::string expected_error_message{"SetErrorInterceptorMulti"};
    ErrorCode expected_error_code{ErrorCode::UNSUPPORTED_PROTOCOL};
    EXPECT_EQ(expected_text, response.front().text);
    EXPECT_EQ(url, response.front().url);
    EXPECT_EQ(expected_status_code, response.front().status_code);
    EXPECT_EQ(expected_error_message, response.front().error.message);
    EXPECT_EQ(expected_error_code, response.front().error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}