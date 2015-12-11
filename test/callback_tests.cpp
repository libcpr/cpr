#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>

#include "server.h"

using namespace cpr;

static Server* server = new Server();
auto base = server->GetBaseUrl();
auto sleep_time = std::chrono::milliseconds(50);
auto zero = std::chrono::seconds(0);

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
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto future = cpr::GetCallback([&status_code] (Response r) {
                status_code = r.status_code;
                return r.status_code;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetLambdaTextTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto future = cpr::GetCallback([&expected_text] (Response r) {
                expected_text = r.text;
                return r.text;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackGetTests, CallbackGetLambdaStatusReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto future = cpr::GetCallback([&status_code] (const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetLambdaTextReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto future = cpr::GetCallback([&expected_text] (const Response& r) {
                expected_text = r.text;
                return r.text;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionStatusTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionTextTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionStatusReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref,
                                                           std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackGetTests, CallbackGetFunctionTextReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::GetCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteLambdaStatusTest) {
    auto url = Url{base + "/delete.html"};
    auto status_code = 0;
    auto future = cpr::DeleteCallback([&status_code] (Response r) {
                status_code = r.status_code;
                return r.status_code;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteLambdaTextTest) {
    auto url = Url{base + "/delete.html"};
    auto expected_text = std::string{};
    auto future = cpr::DeleteCallback([&expected_text] (Response r) {
                expected_text = r.text;
                return r.text;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteLambdaStatusReferenceTest) {
    auto url = Url{base + "/delete.html"};
    auto status_code = 0;
    auto future = cpr::DeleteCallback([&status_code] (const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteLambdaTextReferenceTest) {
    auto url = Url{base + "/delete.html"};
    auto expected_text = std::string{};
    auto future = cpr::DeleteCallback([&expected_text] (const Response& r) {
                expected_text = r.text;
                return r.text;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionStatusTest) {
    auto url = Url{base + "/delete.html"};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionTextTest) {
    auto url = Url{base + "/delete.html"};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionStatusReferenceTest) {
    auto url = Url{base + "/delete.html"};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref,
                                                           std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackDeleteTests, CallbackDeleteFunctionTextReferenceTest) {
    auto url = Url{base + "/delete.html"};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::DeleteCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadLambdaStatusTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto future = cpr::HeadCallback([&status_code] (Response r) {
                status_code = r.status_code;
                return r.status_code;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadLambdaTextTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto future = cpr::HeadCallback([&expected_text] (Response r) {
                expected_text = r.text;
                return r.text;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadLambdaStatusReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto future = cpr::HeadCallback([&status_code] (const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadLambdaTextReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto future = cpr::HeadCallback([&expected_text] (const Response& r) {
                expected_text = r.text;
                return r.text;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionStatusTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionTextTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionStatusReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref,
                                                           std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackHeadTests, CallbackHeadFunctionTextReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::HeadCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostLambdaStatusTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto future = cpr::PostCallback([&status_code] (Response r) {
                status_code = r.status_code;
                return r.status_code;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostLambdaTextTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto future = cpr::PostCallback([&expected_text] (Response r) {
                expected_text = r.text;
                return r.text;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostLambdaStatusReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto future = cpr::PostCallback([&status_code] (const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostLambdaTextReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto future = cpr::PostCallback([&expected_text] (const Response& r) {
                expected_text = r.text;
                return r.text;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionStatusTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionTextTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionStatusReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref,
                                                           std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPostTests, CallbackPostFunctionTextReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::PostCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutLambdaStatusTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto future = cpr::PutCallback([&status_code] (Response r) {
                status_code = r.status_code;
                return r.status_code;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutLambdaTextTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto future = cpr::PutCallback([&expected_text] (Response r) {
                expected_text = r.text;
                return r.text;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutLambdaStatusReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto future = cpr::PutCallback([&status_code] (const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutLambdaTextReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto future = cpr::PutCallback([&expected_text] (const Response& r) {
                expected_text = r.text;
                return r.text;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionStatusTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionTextTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionStatusReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref,
                                                           std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPutTests, CallbackPutFunctionTextReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::PutCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsLambdaStatusTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto future = cpr::OptionsCallback([&status_code] (Response r) {
                status_code = r.status_code;
                return r.status_code;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsLambdaTextTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto future = cpr::OptionsCallback([&expected_text] (Response r) {
                expected_text = r.text;
                return r.text;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsLambdaStatusReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto future = cpr::OptionsCallback([&status_code] (const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsLambdaTextReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto future = cpr::OptionsCallback([&expected_text] (const Response& r) {
                expected_text = r.text;
                return r.text;
            }, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionStatusTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionTextTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionStatusReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref,
                                                           std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackOptionsTests, CallbackOptionsFunctionTextReferenceTest) {
    auto url = Url{base + "/hello.html"};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::OptionsCallback(callback, url);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchLambdaStatusTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto future = cpr::PatchCallback([&status_code] (Response r) {
                status_code = r.status_code;
                return r.status_code;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchLambdaTextTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto future = cpr::PatchCallback([&expected_text] (Response r) {
                expected_text = r.text;
                return r.text;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchLambdaStatusReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto future = cpr::PatchCallback([&status_code] (const Response& r) {
                status_code = r.status_code;
                return r.status_code;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchLambdaTextReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto future = cpr::PatchCallback([&expected_text] (const Response& r) {
                expected_text = r.text;
                return r.text;
            }, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionStatusTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback, std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionTextTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(expected_text, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionStatusReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto status_code = 0;
    auto callback = std::function<int(Response)>(std::bind(status_callback_ref,
                                                           std::ref(status_code),
                                                           std::placeholders::_1));
    auto future = cpr::PatchCallback(callback, url, payload);
    std::this_thread::sleep_for(sleep_time);
    EXPECT_EQ(future.wait_for(zero), std::future_status::ready);
    EXPECT_EQ(status_code, future.get());
}

TEST(CallbackPatchTests, CallbackPatchFunctionTextReferenceTest) {
    auto url = Url{base + "/url_post.html"};
    auto payload = Payload{{"x", "5"}};
    auto expected_text = std::string{};
    auto callback = std::function<std::string(Response)>(std::bind(text_callback_ref,
                                                                   std::ref(expected_text),
                                                                   std::placeholders::_1));
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
