#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cpr/filesystem.h>

#include "cpr/api.h"
#include "cpr/async_wrapper.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

// A cancellable AsyncResponse
using AsyncResponseC = AsyncWrapper<Response, true>;

std::vector<AsyncResponseC> threeHelloWorldReqs () {
    const Url hello_url{server->GetBaseUrl() + "/hello.html"};
    return MultiGetAsync(std::tuple{hello_url}, std::tuple{hello_url}, std::tuple{hello_url});
}

TEST(MultiAsyncBasicTests, MultiAsyncGetTest) {
    const std::string expected_hello{"Hello world!"};
    std::vector resps{threeHelloWorldReqs()};

    for(AsyncResponseC& resp: resps) {
        EXPECT_EQ(expected_hello, resp.get().text);
    }
}
/**
 * We test that cancellation on queue, works, ie libcurl does not get engaged at all
 * To do this, we plant an observer function in the progress call sequence, which
 * will set an atomic boolean to true. The objective is to verify that within 500ms,
 * the function is never called.
 */
TEST(MultiAsyncCancelTests, CancellationOnQueue) {
    const Url hello_url{server->GetBaseUrl() + "/hello.html"};
    std::atomic_bool was_called{false};
    const std::function observer_fn{[&was_called]
            (cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t) -> bool {
                was_called.store(true);
                return true;
            }
    };

    GlobalThreadPool::GetInstance()->Pause();
    std::vector resps{MultiGetAsync(std::tuple{hello_url, ProgressCallback{observer_fn}})};
    EXPECT_EQ(CancellationResult::success, resps.at(0).cancel());
    GlobalThreadPool::GetInstance()->Resume();

    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    EXPECT_EQ(false, was_called);
}

/**
 * We test that cancellation works as intended while the request is being processed by the server.
 * To achieve this we use a condition variable to ensure that the observer function, wrapped in a
 * cpr::ProgressCallback, is called at least once, and then no further calls are made for half a
 * second after cancellation.
 */
TEST(MultiAsyncCancelTests, TestCancellationInTransit) {
    const Url call_url{server->GetBaseUrl() + "/low_speed_bytes.html"};

    std::atomic_size_t counter{0};
    std::condition_variable is_called{};
    std::mutex cv_lock{};
    std::unique_lock setup_lock{cv_lock};
    const std::function observer_fn{[&counter, &is_called, &cv_lock]
        (cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t) -> bool {
            const std::unique_lock l{cv_lock};
            counter++;
            is_called.notify_all();
            return true;
        }
    };
    std::vector res{cpr::MultiGetAsync(std::tuple{call_url, cpr::ProgressCallback{observer_fn}})};
    is_called.wait(setup_lock);
    EXPECT_LT(0, counter);
    EXPECT_EQ(cpr::CancellationResult::success, res.at(0).cancel());
    const size_t calls{counter};

    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    EXPECT_EQ(calls, counter.load());
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

    const size_t N{15};

    // This variable will be used to cancel the transaction at the point of the Nth call.
    std::atomic_size_t counter{0};
    const std::chrono::time_point start{std::chrono::steady_clock::now()};

    const std::function observer_fn{[&counter, &N]
            (cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, cpr_pf_arg_t, intptr_t) -> bool {
                counter++;
                return counter < N;
            }
    };

    std::vector resp{MultiGetAsync(std::tuple{call_url, ProgressCallback{observer_fn}})};
    resp.at(0).wait();

    const std::chrono::duration elapsed_time{std::chrono::steady_clock::now() - start};
    EXPECT_GT(std::chrono::seconds(N), elapsed_time);
}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
