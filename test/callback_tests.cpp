#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>

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
    auto callback = std::function<int(Response)>(
            std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(
            std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
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
    auto callback = std::function<int(Response)>(
            std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(
            std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/delete.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
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
    auto callback = std::function<int(Response)>(
            std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(
            std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
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
    auto callback = std::function<int(Response)>(
            std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(
            std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
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
    auto callback = std::function<int(Response)>(
            std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(
            std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
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
    auto callback = std::function<int(Response)>(
            std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto callback = std::function<int(Response)>(
            std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
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
    auto callback = std::function<int(Response)>(
            std::bind(status_callback, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionTextTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionStatusReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    int status_code = 0;
    auto callback = std::function<int(Response)>(
            std::bind(status_callback_ref, std::ref(status_code), std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionTextReferenceTest) {
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    std::string expected_text{};
    auto callback = std::function<std::string(Response)>(
            std::bind(text_callback_ref, std::ref(expected_text), std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
