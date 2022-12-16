#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>
#include <cpr/filesystem.h>

#include "cpr/api.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

namespace test_internal {

class IntrospectionFunction {
    public:
    explicit IntrospectionFunction(std::function<void()>&& f): introFunc{std::move(f)} {}


    private:
    std::function<void()> introFunc;
};
} // namespace test_internal

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

TEST(MultiAsyncCancelTests, CancellationOnQueue) {
    const Url hello_url{server->GetBaseUrl() + "/hello.html"};
    GlobalThreadPool::GetInstance()->Pause();
    std::vector resps{threeHelloWorldReqs()};
    std::for_each(resps.begin(), resps.end(),
        [] (AsyncResponseC& r) {
            EXPECT_EQ(CancellationResult::success, r.cancel());
        }
    );
    GlobalThreadPool::GetInstance()->Resume();
    // TODO: add assertions here, declare this test a friend function of AsyncWrapper
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
TEST(MultiAsyncCancelTest, TestIntervalOfProgressCallsLowSpeed) {
    const Url call_url{server->GetBaseUrl() + "/low_speed_bytes.html"};

    const size_t N{15};

    // This variable will be used to cancel the transaction at the point of the Nth call.
    const std::shared_ptr no{std::make_shared<std::atomic_size_t>(0)};
    const std::chrono::time_point start{std::chrono::steady_clock::now()};

    std::vector resp{MultiGetAsync(std::tuple{call_url, ProgressCallback{std::function{[no] (cpr_off_t a1, cpr_off_t a2, cpr_off_t a3, cpr_off_t  a4, intptr_t a5) {
        std::ignore = std::tuple(a1, a2, a3, a4, a5);
        no->store(no->load() + 1);
        return no->load() < N;
    }}}})};
    resp.at(0).wait();

    const std::chrono::duration elapsed_time{std::chrono::steady_clock::now() - start};
    EXPECT_GT(std::chrono::seconds(N), elapsed_time);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
