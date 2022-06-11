#include <gtest/gtest.h>
#include <iostream>

#include "cpr/cpr.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

class HiddenHelloWorldRedirectInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        // Save original url
        Url old_url = session.GetFullRequestUrl();

        // Rewrite the url
        Url url{server->GetBaseUrl() + "/hello.html"};
        session.SetUrl(url);

        // Procceed the chain
        Response response = proceed(session);

        // Restore the url again
        response.url = old_url;
        return response;
    }
};

class ChangeStatusCodeInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        // Procceed the chain
        Response response = proceed(session);

        // Change the status code
        response.status_code = 12345;
        return response;
    }
};

class SetBasicAuthInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        // Set authentication
        session.SetAuth(Authentication{"user", "password", AuthMode::BASIC});

        // Procceed the chain
        return proceed(session);
    }
};

class SetUnsupportedProtocolErrorInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        // Procceed the chain
        Response response = proceed(session);

        // Set unsupported protocol error
        response.error = Error{CURLE_UNSUPPORTED_PROTOCOL, "SetErrorInterceptor"};

        // Return response
        return response;
    }
};

class ChangeRequestMethodToGetInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        return proceed(session, Interceptor::ProceedHttpMethod::GET_REQUEST);
    }
};

class ChangeRequestMethodToPostInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        session.SetOption(Payload{{"x", "5"}});
        return proceed(session, Interceptor::ProceedHttpMethod::POST_REQUEST);
    }
};

class ChangeRequestMethodToPutInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        session.SetOption(Payload{{"x", "5"}});
        return proceed(session, Interceptor::ProceedHttpMethod::PUT_REQUEST);
    }
};

class ChangeRequestMethodToDeleteInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        return proceed(session, Interceptor::ProceedHttpMethod::DELETE_REQUEST);
    }
};

bool write_data(std::string /*data*/, intptr_t /*userdata*/) {
    return true;
}

class ChangeRequestMethodToDownloadCallbackInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        return proceed(session, Interceptor::ProceedHttpMethod::DOWNLOAD_CALLBACK_REQUEST, WriteCallback{write_data, 0});
    }
};

class ChangeRequestMethodToHeadInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        return proceed(session, Interceptor::ProceedHttpMethod::HEAD_REQUEST);
    }
};

class ChangeRequestMethodToOptionsInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        return proceed(session, Interceptor::ProceedHttpMethod::OPTIONS_REQUEST);
    }
};

class ChangeRequestMethodToPatchInterceptor : public Interceptor {
  public:
    Response intercept(Session& session) override {
        session.SetOption(Payload{{"x", "5"}});
        return proceed(session, Interceptor::ProceedHttpMethod::PATCH_REQUEST);
    }
};

TEST(InterceptorTest, HiddenUrlRewriteInterceptorTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<HiddenHelloWorldRedirectInterceptor>());
    Response response = session.Get();

    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, ChangeStatusCodeInterceptorTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeStatusCodeInterceptor>());
    Response response = session.Get();

    long expected_status_code{12345};
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(expected_status_code, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, DownloadChangeStatusCodeInterceptorTest) {
    cpr::Url url{server->GetBaseUrl() + "/download_gzip.html"};
    cpr::Session session;
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeStatusCodeInterceptor>());
    Response response = session.Download(cpr::WriteCallback{write_data, 0});

    long expected_status_code{12345};
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(expected_status_code, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, SetBasicAuthInterceptorTest) {
    Url url{server->GetBaseUrl() + "/basic_auth.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<SetBasicAuthInterceptor>());

    Response response = session.Get();
    std::string expected_text{"Header reflect GET"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, SetUnsupportedProtocolErrorInterceptorTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<SetUnsupportedProtocolErrorInterceptor>());

    Response response = session.Get();
    std::string expected_error_message{"SetErrorInterceptor"};
    ErrorCode expected_error_code{ErrorCode::UNSUPPORTED_PROTOCOL};
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(expected_error_message, response.error.message);
    EXPECT_EQ(expected_error_code, response.error.code);
}

TEST(InterceptorTest, ChangeRequestMethodToGetInterceptorTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeRequestMethodToGetInterceptor>());
    Response response = session.Head();

    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, ChangeRequestMethodToPostInterceptorTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeRequestMethodToPostInterceptor>());
    Response response = session.Head();

    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, ChangeRequestMethodToPutInterceptorTest) {
    Url url{server->GetBaseUrl() + "/put.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeRequestMethodToPutInterceptor>());
    Response response = session.Get();

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

TEST(InterceptorTest, ChangeRequestMethodToPatchInterceptorTest) {
    Url url{server->GetBaseUrl() + "/patch.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeRequestMethodToPatchInterceptor>());
    Response response = session.Get();

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

TEST(InterceptorTest, ChangeRequestMethodToOptionsInterceptorTest) {
    Url url{server->GetBaseUrl() + "/"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeRequestMethodToOptionsInterceptor>());
    Response response = session.Get();

    std::string expected_text{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, ChangeRequestMethodToHeadInterceptorTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeRequestMethodToHeadInterceptor>());
    Response response = session.Get();

    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, ChangeRequestMethodToDownloadCallbackInterceptorTest) {
    Url url{server->GetBaseUrl() + "/download_gzip.html"};
    Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
    session.SetTimeout(Timeout{2000});
    session.AddInterceptor(std::make_shared<ChangeRequestMethodToDownloadCallbackInterceptor>());
    Response response = session.Put();

    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, ChangeRequestMethodToDeleteInterceptorTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<ChangeRequestMethodToDeleteInterceptor>());
    Response response = session.Get();


    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, TwoInterceptorsTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<HiddenHelloWorldRedirectInterceptor>());
    session.AddInterceptor(std::make_shared<ChangeStatusCodeInterceptor>());
    Response response = session.Get();

    std::string expected_text{"Hello world!"};
    long expected_status_code{12345};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(expected_status_code, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(InterceptorTest, ThreeInterceptorsTest) {
    Url url{server->GetBaseUrl() + "/basic.json"};
    Session session;
    session.SetUrl(url);
    session.AddInterceptor(std::make_shared<HiddenHelloWorldRedirectInterceptor>());
    session.AddInterceptor(std::make_shared<ChangeStatusCodeInterceptor>());
    session.AddInterceptor(std::make_shared<SetUnsupportedProtocolErrorInterceptor>());
    Response response = session.Get();

    std::string expected_text{"Hello world!"};
    long expected_status_code{12345};
    std::string expected_error_message{"SetErrorInterceptor"};
    ErrorCode expected_error_code{ErrorCode::UNSUPPORTED_PROTOCOL};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(expected_status_code, response.status_code);
    EXPECT_EQ(expected_error_message, response.error.message);
    EXPECT_EQ(expected_error_code, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}