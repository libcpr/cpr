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
        Response response = session.proceed();

        // Restore the url again
        response.url = old_url;
        return response;
    };
};

TEST(InterceptorTest, HiddenUrlRewriteInterceptor) {
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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}