#include <functional>
#include <string>
#include <vector>

#include "cpr/cpr.h"

#include "httpServer.hpp"
#include "multiasync_tests.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

// A cancellable AsyncResponse
using AsyncResponseC = AsyncWrapper<Response, true>;

/** This property is tested at compile-time, so if compilation succeeds, it has already been verified. It is, however, useful to structure it as a test for semantic purposes.
 */
TEST(AsyncWrapperTests, TestConstructorDeductions) {
    auto wrapper_non_cancellable{AsyncWrapper{std::future<std::string>{}}};
    auto wrapper_cancellable{AsyncWrapper{std::future<std::string>{}, std::make_shared<std::atomic_bool>(false)}};

    static_assert(std::is_same_v<AsyncWrapper<std::string, false>, decltype(wrapper_non_cancellable)>);
    static_assert(std::is_same_v<AsyncWrapper<std::string, true>, decltype(wrapper_cancellable)>);
    SUCCEED();
}

/** These tests aim to set a point of reference for AsyncWrapper behavior.
 * Those functions that replicate std::future member functions should behave in a way that is in all ways compatible.
 * Others should behave as expected by the below test set.
 */
TEST(AsyncWrapperNonCancellableTests, TestGetNoError) {
    const Url hello_url{server->GetBaseUrl() + "/hello.html"};
    const std::string expected_hello{"Hello world!"};
    const Response resp{GetAsync(hello_url).get()};
    EXPECT_EQ(expected_hello, resp.text);
}

TEST(AsyncWrapperNonCancellableTests, TestExceptionsNoSharedState) {
    const std::chrono::duration five_secs{std::chrono::seconds(1)};
    const std::chrono::time_point in_five_s{std::chrono::steady_clock::now() + five_secs};

    // We create an AsyncWrapper for a future without a shared state (default-initialized)
    AsyncWrapper test_wrapper{std::future<std::string>{}};


    ASSERT_FALSE(test_wrapper.valid());
    ASSERT_FALSE(test_wrapper.IsCancelled());

    // Trying to get or wait for a future that doesn't have a shared state should result to an exception
    // It should be noted that there is a divergence from std::future behavior here: calling wait* on the original std::future is undefined behaviour, according to cppreference.com . We find it preferrable to throw an exception.
    EXPECT_THROW(std::ignore = test_wrapper.get(), std::exception);
    EXPECT_THROW(test_wrapper.wait(), std::exception);
    EXPECT_THROW(test_wrapper.wait_for(five_secs), std::exception);
    EXPECT_THROW(test_wrapper.wait_until(in_five_s), std::exception);
}

TEST(AsyncWrapperCancellableTests, TestExceptionsNoSharedState) {
    const std::chrono::duration five_secs{std::chrono::seconds(5)};
    const std::chrono::time_point in_five_s{std::chrono::steady_clock::now() + five_secs};

    AsyncWrapper test_wrapper{std::future<std::string>{}, std::make_shared<std::atomic_bool>(false)};

    static_assert(std::is_same<AsyncWrapper<std::string, true>, decltype(test_wrapper)>::value);

    ASSERT_FALSE(test_wrapper.valid());
    ASSERT_FALSE(test_wrapper.IsCancelled());

    EXPECT_THROW(std::ignore = test_wrapper.get(), std::exception);
    EXPECT_THROW(test_wrapper.wait(), std::exception);
    EXPECT_THROW(test_wrapper.wait_for(five_secs), std::exception);
    EXPECT_THROW(test_wrapper.wait_until(in_five_s), std::exception);
}

TEST(AsyncWrapperCancellableTests, TestExceptionsCancelledRequest) {
    const Url call_url{server->GetBaseUrl() + "/low_speed_bytes.html"};
    const std::chrono::duration five_secs{std::chrono::seconds(5)};
    const std::chrono::time_point in_five_s{std::chrono::steady_clock::now() + five_secs};

    AsyncResponseC test_wrapper{std::move(MultiGetAsync(std::tuple{call_url}).at(0))};
    EXPECT_EQ(CancellationResult::success, test_wrapper.Cancel());
    EXPECT_EQ(CancellationResult::invalid_operation, test_wrapper.Cancel());
    ASSERT_TRUE(test_wrapper.IsCancelled());

    EXPECT_THROW(std::ignore = test_wrapper.get(), std::exception);
    EXPECT_THROW(test_wrapper.wait(), std::exception);
    EXPECT_THROW(test_wrapper.wait_for(five_secs), std::exception);
    EXPECT_THROW(test_wrapper.wait_until(in_five_s), std::exception);
}

TEST(AsyncWrapperCancellableTests, TestWaitFor) {
    constexpr std::chrono::duration wait_for_time{std::chrono::milliseconds(100)};
    constexpr std::chrono::duration teardown_time{std::chrono::milliseconds(10)};

    const Url call_url{server->GetBaseUrl() + "/low_speed_bytes.html"};

    AsyncResponseC test_wrapper{std::move(MultiGetAsync(std::tuple{call_url}).at(0))};

    EXPECT_EQ(std::future_status::timeout, test_wrapper.wait_for(wait_for_time));

    ASSERT_TRUE(test_wrapper.valid());
    ASSERT_FALSE(test_wrapper.IsCancelled());

    EXPECT_EQ(CancellationResult::success, test_wrapper.Cancel());

    std::this_thread::sleep_for(teardown_time);
}
/** The group MultiAsyncBasicTests executes multiple tests from the test sources associated with every Http action in parallel.
 * These tests are reproductions of tests from the appropriate test suites, but they guarantee that the multiasync function template produces correctly working instantiations for every Http action.
 */
TEST(MultiAsyncBasicTests, MultiAsyncGetTest) {
    const Url hello_url{server->GetBaseUrl() + "/hello.html"};
    const std::string expected_hello{"Hello world!"};
    std::vector<AsyncResponseC> resps{MultiGetAsync(std::tuple{hello_url}, std::tuple{hello_url}, std::tuple{hello_url})};

    for (AsyncResponseC& resp : resps) {
        EXPECT_EQ(expected_hello, resp.get().text);
    }
}

TEST(MultiAsyncBasicTests, MultiAsyncDeleteTest) {
    const std::string server_base{server->GetBaseUrl()};
    const Url delete_allowed{server_base + "/delete.html"};
    const Url delete_unallowed{server_base + "/delete_unallowed.html"};
    const std::tuple del_json_params{delete_allowed, Body{"'foo':'bar'"}, Header{{"Content-Type", "application/json"}}};
    const std::string expected_text_success{"Delete success"};
    const std::string expected_text_fail{"Method Not Allowed"};
    const std::string expected_text_json{"'foo':'bar'"};

    std::vector<AsyncResponseC> resps{MultiDeleteAsync(std::tuple{delete_allowed}, std::tuple{delete_unallowed}, del_json_params)};

    Response del_success{resps.at(0).get()};
    Response del_fail{resps.at(1).get()};
    Response del_json{resps.at(2).get()};

    EXPECT_EQ(expected_text_success, del_success.text);
    EXPECT_EQ(delete_allowed, del_success.url);
    EXPECT_EQ(std::string{"text/html"}, del_success.header["content-type"]);
    EXPECT_EQ(200, del_success.status_code);
    EXPECT_EQ(ErrorCode::OK, del_success.error.code);

    EXPECT_EQ(expected_text_fail, del_fail.text);
    EXPECT_EQ(delete_unallowed, del_fail.url);
    EXPECT_EQ(std::string{"text/plain"}, del_fail.header["content-type"]);
    EXPECT_EQ(405, del_fail.status_code);
    EXPECT_EQ(ErrorCode::OK, del_fail.error.code);

    EXPECT_EQ(expected_text_json, del_json.text);
    EXPECT_EQ(delete_allowed, del_json.url);
    EXPECT_EQ(std::string{"application/json"}, del_json.header["content-type"]);
    EXPECT_EQ(200, del_json.status_code);
    EXPECT_EQ(ErrorCode::OK, del_json.error.code);
}

TEST(MultiAsyncBasicTests, MultiAsyncHeadTest) {
    const std::string server_base{server->GetBaseUrl()};
    const Url hello_url{server_base + "/hello.html"};
    const Url json_url{server_base + "/basic.json"};
    const Url notfound_url{server_base + "/error.html"};
    const Url digest_url{server_base + "/digest_auth.html"};
    const Authentication digest_auth{"user", "password", AuthMode::DIGEST};

    std::vector<AsyncResponseC> resps{MultiHeadAsync(std::tuple{hello_url}, std::tuple{json_url}, std::tuple{notfound_url}, std::tuple{digest_url, digest_auth})};
    Response hello_resp{resps.at(0).get()};
    Response json_resp{resps.at(1).get()};
    Response notfound_resp{resps.at(2).get()};
    Response digest_resp{resps.at(3).get()};

    EXPECT_EQ(std::string{}, hello_resp.text);
    EXPECT_EQ(hello_url, hello_resp.url);
    EXPECT_EQ(std::string{"text/html"}, hello_resp.header["content-type"]);
    EXPECT_EQ(200, hello_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, hello_resp.error.code);

    EXPECT_EQ(std::string{}, json_resp.text);
    EXPECT_EQ(json_url, json_resp.url);
    EXPECT_EQ(std::string{"application/json"}, json_resp.header["content-type"]);
    EXPECT_EQ(200, json_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, json_resp.error.code);

    EXPECT_EQ(std::string{}, notfound_resp.text);
    EXPECT_EQ(notfound_url, notfound_resp.url);
    EXPECT_EQ(std::string{"text/plain"}, notfound_resp.header["content-type"]);
    EXPECT_EQ(404, notfound_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, notfound_resp.error.code);

    EXPECT_EQ(std::string{}, digest_resp.text);
    EXPECT_EQ(digest_url, digest_resp.url);
    EXPECT_EQ(std::string{"text/html"}, digest_resp.header["content-type"]);
    EXPECT_EQ(200, digest_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, digest_resp.error.code);
}

TEST(MultiAsyncBasicTests, MultiAsyncOptionsTest) {
    const std::string server_base{server->GetBaseUrl()};
    const Url root_url{server_base + "/"};
    const Url hello_url{server_base + "/hello.html"};

    std::vector<AsyncResponseC> resps{MultiOptionsAsync(std::tuple{root_url}, std::tuple{hello_url})};

    Response root_resp{resps.at(0).get()};
    Response hello_resp{resps.at(1).get()};

    EXPECT_EQ(std::string{}, root_resp.text);
    EXPECT_EQ(root_url, root_resp.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, root_resp.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, root_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, root_resp.error.code);

    EXPECT_EQ(std::string{}, hello_resp.text);
    EXPECT_EQ(hello_url, hello_resp.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, hello_resp.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, hello_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, hello_resp.error.code);
}

TEST(MultiAsyncBasicTests, MultiAsyncPatchTest) {
    const std::string server_base{server->GetBaseUrl()};
    const Url patch_url{server_base + "/patch.html"};
    const Url patch_not_allowed_url{server_base + "/patch_unallowed.html"};
    const Payload pl{{"x", "10"}, {"y", "1"}};
    const std::string expected_text{
            "{\n"
            "  \"x\": 10,\n"
            "  \"y\": 1,\n"
            "  \"sum\": 11\n"
            "}"};
    const std::string notallowed_text{"Method Not Allowed"};
    std::vector<AsyncResponseC> resps{MultiPatchAsync(std::tuple{patch_url, pl}, std::tuple{patch_not_allowed_url, pl})};
    const Response success{resps.at(0).get()};
    const Response fail{resps.at(1).get()};
    EXPECT_EQ(expected_text, success.text);
    EXPECT_EQ(200, success.status_code);
    EXPECT_EQ(patch_url, success.url);

    EXPECT_EQ(notallowed_text, fail.text);
    EXPECT_EQ(405, fail.status_code);
    EXPECT_EQ(ErrorCode::OK, fail.error.code);
}

TEST(MultiAsyncBasicTests, MultiAsyncPostTest) {
    const std::string server_base{server->GetBaseUrl()};
    const Url post_url{server_base + "/url_post.html"};
    const Url form_post_url{server_base + "/form_post.html"};

    const Payload post_data{{"x", "5"}, {"y", "15"}};
    const Multipart form_data{{"x", 5}};

    const std::string post_text{
            "{\n"
            "  \"x\": 5,\n"
            "  \"y\": 15,\n"
            "  \"sum\": 20\n"
            "}"};
    const std::string form_text{
            "{\n"
            "  \"x\": \"5\"\n"
            "}"};

    std::vector<AsyncResponseC> resps{MultiPostAsync(std::tuple{post_url, post_data}, std::tuple{form_post_url, form_data})};

    Response post_resp{resps.at(0).get()};
    Response form_resp{resps.at(1).get()};

    EXPECT_EQ(post_text, post_resp.text);
    EXPECT_EQ(post_url, post_resp.url);
    EXPECT_EQ(std::string{"application/json"}, post_resp.header["content-type"]);
    EXPECT_EQ(201, post_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, post_resp.error.code);

    EXPECT_EQ(form_text, form_resp.text);
    EXPECT_EQ(form_post_url, form_resp.url);
    EXPECT_EQ(std::string{"application/json"}, form_resp.header["content-type"]);
    EXPECT_EQ(201, form_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, form_resp.error.code);
}

TEST(MultiAsyncBasicTests, MultiAsyncPutTest) {
    const std::string server_base{server->GetBaseUrl()};
    const Url put_url{server_base + "/put.html"};
    const Url put_failure_url{server_base + "/put_unallowed.html"};
    const Payload pl{{"x", "7"}};
    const std::string success_text{
            "{\n"
            "  \"x\": 7\n"
            "}"};
    const std::string failure_text{"Method Not Allowed"};

    std::vector<AsyncResponseC> resps{MultiPutAsync(std::tuple{put_url, pl}, std::tuple{put_failure_url, pl})};
    Response success_resp{resps.at(0).get()};
    Response failure_resp{resps.at(1).get()};

    EXPECT_EQ(success_text, success_resp.text);
    EXPECT_EQ(put_url, success_resp.url);
    EXPECT_EQ(std::string{"application/json"}, success_resp.header["content-type"]);
    EXPECT_EQ(200, success_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, success_resp.error.code);

    EXPECT_EQ(failure_text, failure_resp.text);
    EXPECT_EQ(put_failure_url, failure_resp.url);
    EXPECT_EQ(std::string{"text/plain"}, failure_resp.header["content-type"]);
    EXPECT_EQ(405, failure_resp.status_code);
    EXPECT_EQ(ErrorCode::OK, failure_resp.error.code);
}

static TestSynchronizationEnv* synchro_env = new TestSynchronizationEnv();

/**
 * We test that cancellation on queue, works, ie libcurl does not get engaged at all
 * To do this, we plant an observer function in the progress call sequence, which
 * will set an atomic boolean to true. The objective is to verify that within 500ms,
 * the function is never called.
 */
TEST(MultiAsyncCancelTests, CancellationOnQueue) {
    synchro_env->Reset();
    const Url hello_url{server->GetBaseUrl() + "/hello.html"};
    const std::function<bool(cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t)> observer_fn{[](cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t) -> bool {
        synchro_env->fn_called.store(true);
        return true;
    }};

    GlobalThreadPool::GetInstance()->Pause();
    std::vector<AsyncResponseC> resps{MultiGetAsync(std::tuple{hello_url, ProgressCallback{observer_fn}})};
    EXPECT_EQ(CancellationResult::success, resps.at(0).Cancel());
    GlobalThreadPool::GetInstance()->Resume();
    const bool was_called{synchro_env->fn_called};
    EXPECT_EQ(false, was_called);
}

/**
 * We test that cancellation works as intended while the request is being processed by the server.
 * To achieve this we use a condition variable to ensure that the observer function, wrapped in a
 * cpr::ProgressCallback, is called at least once, and then no further calls are made for half a
 * second after cancellation.
 *
 * The usage of the condition variable and mutex to synchronize this procedure is analogous to the section "Example" in https://en.cppreference.com/w/cpp/thread/condition_variable
 * We use the condition variable in our synchronization environment to ensure that the transfer has
 * started at the time of cancellation, ie the observer function has been called at least once.
 */
TEST(MultiAsyncCancelTests, TestCancellationInTransit) {
    const Url call_url{server->GetBaseUrl() + "/low_speed_bytes.html"};
    synchro_env->Reset();

    // 1. Thread running the test acquires the condition variable's mutex
    std::unique_lock setup_lock{synchro_env->test_cv_mutex};
    const std::function<bool(cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t)> observer_fn{[](cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t) -> bool {
        if (synchro_env->counter == 0) {
            // 3. in Threadpool, the cv mutex is obtained by the worker thread
            const std::unique_lock l{synchro_env->test_cv_mutex};
            synchro_env->counter++;
            // 4. the cv is notified
            synchro_env->test_cv.notify_all();
        } else {
            synchro_env->counter++;
        }
        return true;
    }};
    std::vector<AsyncResponseC> res{cpr::MultiGetAsync(std::tuple{call_url, cpr::ProgressCallback{observer_fn}})};
    // 2. cv mutex is released, thread waits for notification on cv
    // see https://en.cppreference.com/w/cpp/thread/condition_variable/wait
    synchro_env->test_cv.wait(setup_lock);
    // 5. execution continues after notification
    const size_t init_calls{synchro_env->counter};
    EXPECT_LT(0, init_calls);
    EXPECT_EQ(cpr::CancellationResult::success, res.at(0).Cancel());
    const size_t calls{synchro_env->counter};
    std::this_thread::sleep_for(std::chrono::milliseconds{101});
    const size_t calls_post{synchro_env->counter};
    EXPECT_LT(calls_post, calls + 2);
}

/** Checks that the request is cancelled when the corresponding AsyncResponseC is desturcted
 */
TEST(MultiAsyncCancelTests, TestCancellationOnResponseWrapperDestruction) {
    const Url call_url{server->GetBaseUrl() + "/hello.html"};
    synchro_env->Reset();
    std::unique_lock setup_lock{synchro_env->test_cv_mutex};
    const std::function<bool(cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t)> observer_fn{[](cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t) -> bool {
        const std::unique_lock l{synchro_env->test_cv_mutex};
        synchro_env->counter++;
        synchro_env->test_cv.notify_all();
        return true;
    }};

    // We construct a Request that will not terminate, wait until it is being processed by a thread, and destruct the AsyncResponseC
    {
        AsyncResponseC resp{std::move(MultiGetAsync(std::tuple{call_url, ProgressCallback{observer_fn}}).at(0))};
        synchro_env->test_cv.wait(setup_lock);
        const size_t init_calls{synchro_env->counter};
        EXPECT_LT(0, init_calls);
    }

    const size_t calls{synchro_env->counter};
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    const size_t post_calls{synchro_env->counter};
    EXPECT_EQ(calls, post_calls);
}

/**
 * This test checks if the interval of calls to the progress function is
 * acceptable during a low-speed transaction. The server's low_speed_bytes
 * uri sends 1 Byte/second, and we aim to evaluate that 15 calls to the
 * progress function happen within 5 seconds. This would indicate that
 * the user can realistically expect to have their request cancelled within
 * ~1s on a bad case (low network speed).
 * INFO this test is not, strictly speaking, deterministic. It depends at the
 * least on scheduler behaviour. We have tried, however, to set a boundary that
 * is permissive enough to ensure consistency.
 */

TEST(MultiAsyncCancelTests, TestIntervalOfProgressCallsLowSpeed) {
    const Url call_url{server->GetBaseUrl() + "/low_speed_bytes.html"};

    synchro_env->Reset();
    size_t N{15};
    // This variable will be used to cancel the transaction at the point of the Nth call.
    const std::chrono::time_point start{std::chrono::steady_clock::now()};

    const std::function<bool(cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t)> observer_fn{[N](cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t) -> bool {
        const size_t current_iteration{++(synchro_env->counter)};
        return current_iteration <= N;
    }};
    const ProgressCallback pcall{observer_fn};

    std::vector<AsyncResponseC> resp{MultiGetAsync(std::tuple{call_url, pcall})};
    resp.at(0).wait();

    const std::chrono::duration elapsed_time{std::chrono::steady_clock::now() - start};
    EXPECT_GT(std::chrono::seconds(N), elapsed_time);
    std::this_thread::sleep_for(std::chrono::milliseconds{101});
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    ::testing::AddGlobalTestEnvironment(synchro_env);
    return RUN_ALL_TESTS();
}
