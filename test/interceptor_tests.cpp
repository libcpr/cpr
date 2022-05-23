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