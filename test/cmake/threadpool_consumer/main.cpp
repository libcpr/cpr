#include <cpr/threadpool.h>
#ifdef CPR_TEST_CUSTOM_CONTEXT
#include <boost/asio/io_context.hpp>
#include <cpr/async.h>
#endif
#include <cpr/threadpool_backend.h>

int main() {
    cpr::ThreadPool pool(0, 1);
    auto result = pool.Submit([] { return 42; });
    pool.Wait();
    if (result.get() != 42) {
        return 1;
    }
#ifdef CPR_TEST_CUSTOM_CONTEXT
    boost::asio::io_context context;
    cpr::async::startup(context, 0, 1);
    auto externalResult = cpr::async([] { return 17; });
    context.run();
    const int value = externalResult.get();
    cpr::async::cleanup();
    return value == 17 ? 0 : 1;
#else
    return 0;
#endif
}
