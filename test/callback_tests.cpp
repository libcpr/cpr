#include <cstddef>
#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <cpr/cpr.h>

#include "cpr/cprtypes.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();
std::chrono::milliseconds sleep_time{50};
std::chrono::seconds zero{0};

int status_callback(int& status_code, Response r) {
    status_code = r.status_code;
    return r.status_code;
}

int status_callback_ref(int& status_code, const Response& r) {
    status_code = r.status_code;
    return r.status_code;
}

std::string text_callback(std::string& expected_text, Response r) {
    expected_text = r.text;
    return r.text;
}

std::string text_callback_ref(std::string& expected_text, const Response& r) {
    expected_text = r.text;
    return r.text;
}

TEST(CallbackGetTests, CallbackGetLambdaStatusTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto future = cpr::GetCallback(
            [&status_code](Response r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetLambdaTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto future = cpr::GetCallback(
            [&expected_text](Response r) {
                expected_text = r.text;
                return r.text;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackGetTests, CallbackGetLambdaStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto future = cpr::GetCallback(
            [&status_code](const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetLambdaTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto future = cpr::GetCallback(
            [&expected_text](const Response& r) {
                expected_text = r.text;
                return r.text;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionStatusTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteLambdaStatusTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    int status_code = 0;
    auto future = cpr::DeleteCallback(
            [&status_code](Response r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteLambdaTextTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::string expected_text{};
    auto future = cpr::DeleteCallback(
            [&expected_text](Response r) {
                expected_text = r.text;
                return r.text;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteLambdaStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    int status_code = 0;
    auto future = cpr::DeleteCallback(
            [&status_code](const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteLambdaTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::string expected_text{};
    auto future = cpr::DeleteCallback(
            [&expected_text](const Response& r) {
                expected_text = r.text;
                return r.text;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionStatusTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadLambdaStatusTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto future = cpr::HeadCallback(
            [&status_code](Response r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadLambdaTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto future = cpr::HeadCallback(
            [&expected_text](Response r) {
                expected_text = r.text;
                return r.text;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadLambdaStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto future = cpr::HeadCallback(
            [&status_code](const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadLambdaTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto future = cpr::HeadCallback(
            [&expected_text](const Response& r) {
                expected_text = r.text;
                return r.text;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionStatusTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostLambdaStatusTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto future = cpr::PostCallback(
            [&status_code](Response r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostLambdaTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto future = cpr::PostCallback(
            [&expected_text](Response r) {
                expected_text = r.text;
                return r.text;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostLambdaStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto future = cpr::PostCallback(
            [&status_code](const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostLambdaTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto future = cpr::PostCallback(
            [&expected_text](const Response& r) {
                expected_text = r.text;
                return r.text;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionStatusTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutLambdaStatusTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto future = cpr::PutCallback(
            [&status_code](Response r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutLambdaTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto future = cpr::PutCallback(
            [&expected_text](Response r) {
                expected_text = r.text;
                return r.text;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutLambdaStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto future = cpr::PutCallback(
            [&status_code](const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutLambdaTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto future = cpr::PutCallback(
            [&expected_text](const Response& r) {
                expected_text = r.text;
                return r.text;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionStatusTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsLambdaStatusTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto future = cpr::OptionsCallback(
            [&status_code](Response r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsLambdaTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto future = cpr::OptionsCallback(
            [&expected_text](Response r) {
                expected_text = r.text;
                return r.text;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsLambdaStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto future = cpr::OptionsCallback(
            [&status_code](const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsLambdaTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto future = cpr::OptionsCallback(
            [&expected_text](const Response& r) {
                expected_text = r.text;
                return r.text;
            },
            url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionStatusTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchLambdaStatusTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto future = cpr::PatchCallback(
            [&status_code](Response r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchLambdaTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto future = cpr::PatchCallback(
            [&expected_text](Response r) {
                expected_text = r.text;
                return r.text;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchLambdaStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto future = cpr::PatchCallback(
            [&status_code](const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchLambdaTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto future = cpr::PatchCallback(
            [&expected_text](const Response& r) {
                expected_text = r.text;
                return r.text;
            },
            url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionStatusTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDataTests, CallbackReadFunctionCancelTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = cpr::Post(url, cpr::ReadCallback([](char* /*buffer*/, size_t& /*size*/) -> size_t { return false; }));
    EXPECT_EQ(response.error.code, ErrorCode::REQUEST_CANCELLED);
}

TEST(CallbackDataTests, CallbackReadFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    unsigned count = 0;
    Response response = cpr::Post(url, cpr::ReadCallback{3, [&](char* buffer, size_t& size) -> size_t {
                                                             std::string data;
                                                             ++count;
                                                             switch (count) {
                                                                 case 1:
                                                                     data = "x=";
                                                                     break;
                                                                 case 2:
                                                                     data = "5";
                                                                     break;
                                                                 default:
                                                                     return false;
                                                             }
                                                             std::copy(data.begin(), data.end(), buffer);
                                                             size = data.size();
                                                             return true;
                                                         }});
    EXPECT_EQ(2, count);
    EXPECT_EQ(expected_text, response.text);
}

/**
 * Checks if the "Transfer-Encoding" header will be kept when using headers and a read callback.
 * Issue: https://github.com/whoshuu/cpr/issues/517
 **/
TEST(CallbackDataTests, CallbackReadFunctionHeaderTest) {
    Url url{server->GetBaseUrl() + "/header_reflect.html"};
    std::string data = "Test";
    Response response = cpr::Post(url,
                                  cpr::ReadCallback{-1,
                                                    [&](char* /*buffer*/, size_t& size) -> size_t {
                                                        size = 0;
                                                        return true;
                                                    }},
                                  Header{{"TestHeader", "42"}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);

    // Check Header:
    EXPECT_EQ(std::string{"42"}, response.header["TestHeader"]); // Set by us
    EXPECT_TRUE(response.header.find("TestHeader") != response.header.end());
    EXPECT_EQ(std::string{"chunked"}, response.header["Transfer-Encoding"]); // Set by the read callback
    EXPECT_TRUE(response.header.find("Transfer-Encoding") != response.header.end());
}

/* cesanta mongoose doesn't support chunked requests yet
TEST(CallbackDataTests, CallbackReadFunctionChunkedTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    unsigned count = 0;
    Response response = cpr::Post(url, cpr::ReadCallback{[&count](char* buffer, size_t & size) -> size_t {
        std::string data;
        ++ count;
        switch (count) {
        case 1:
            data = "x=";
            break;
        case 2:
            data = "5";
            break;
        default:
            data = "";
            break;
        }
        std::copy(data.begin(), data.end(), buffer);
        size = data.size();
        return true;
    }});
    EXPECT_EQ(3, count);
    EXPECT_EQ(expected_text, response.text);
}
*/

TEST(CallbackDataTests, CallbackHeaderFunctionCancelTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = Post(url, HeaderCallback{[](std::string /*header*/) -> bool { return false; }});
    EXPECT_EQ(response.error.code, ErrorCode::REQUEST_CANCELLED);
}

TEST(CallbackDataTests, CallbackHeaderFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    std::vector<std::string> expected_headers{"HTTP/1.1 201 OK\r\n", "Content-Type: application/json\r\n", "\r\n"};
    std::set<std::string> response_headers;
    Post(url, HeaderCallback{[&response_headers](std::string header) -> bool {
             response_headers.insert(header);
             return true;
         }});
    for (std::string& header : expected_headers) {
        EXPECT_TRUE(response_headers.count(header));
    }
}

TEST(CallbackDataTests, CallbackWriteFunctionCancelTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = Post(url, WriteCallback{[](std::string /*header*/) -> bool { return false; }});
    EXPECT_EQ(response.error.code, ErrorCode::REQUEST_CANCELLED);
}

TEST(CallbackDataTests, CallbackWriteFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    std::string response_text;
    Post(url, Payload{{"x", "5"}}, WriteCallback{[&response_text](std::string header) -> bool {
             response_text.append(header);
             return true;
         }});
    EXPECT_EQ(expected_text, response_text);
}

TEST(CallbackDataTests, CallbackProgressFunctionCancelTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Response response = Post(url, ProgressCallback{[](size_t /*downloadTotal*/, size_t /*downloadNow*/, size_t /*uploadTotal*/, size_t /*uploadNow*/) -> bool { return false; }});
    EXPECT_EQ(response.error.code, ErrorCode::REQUEST_CANCELLED);
}

TEST(CallbackDataTests, CallbackProgressFunctionTotalTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Body body{"x=5"};
    size_t response_upload = 0, response_download = 0;
    Response response = Post(url, body, ProgressCallback{[&](size_t downloadTotal, size_t /*downloadNow*/, size_t uploadTotal, size_t /*uploadNow*/) -> bool {
                                 response_upload = uploadTotal;
                                 response_download = downloadTotal;
                                 return true;
                             }});
    EXPECT_EQ(body.str().length(), response_upload);
    EXPECT_EQ(response.text.length(), response_download);
}

TEST(CallbackDataTests, CallbackDebugFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Body body{"x=5"};
    std::string debug_body;
    Response response = Post(url, body, DebugCallback{[&](DebugCallback::InfoType type, std::string data) {
                                 if (type == DebugCallback::InfoType::DATA_OUT) {
                                     debug_body = data;
                                 }
                             }});
    EXPECT_EQ(body.str(), debug_body);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
