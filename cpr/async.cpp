#include "cpr/async.h"
#include "cpr/threadpool.h"
#include "cpr/threadpool_backend.h"

#include <cstddef>
#include <mutex>
#include <stdexcept>

namespace cpr {

// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
CPR_SINGLETON_IMPL(GlobalThreadPool)

GlobalThreadPool* GlobalThreadPool::GetInstance(boost::asio::io_context& context) {
    std::call_once(s_getFlag, [&context] { s_pInstance = new GlobalThreadPool(context); });
    if (s_pInstance == nullptr || s_pInstance->context != &context) {
        throw std::logic_error("The global thread pool must select its Asio context on first initialization and cannot restart after cleanup");
    }
    return s_pInstance;
}

void async::startup(boost::asio::io_context& context, size_t minThreads, size_t maxThreads) {
    ThreadPoolBackend::validate(minThreads, maxThreads);
    GlobalThreadPool::GetInstance(context)->SetThreadCounts(minThreads, maxThreads);
}

} // namespace cpr
