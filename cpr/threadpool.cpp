#include "cpr/threadpool.h"
#include "cpr/threadpool_backend.h"

#include <algorithm>
#include <thread>

namespace cpr {
// NOLINTNEXTLINE(cert-err58-cpp) Not relevant since trivial function.
size_t ThreadPool::DEFAULT_MAX_THREAD_COUNT = std::max<size_t>(std::thread::hardware_concurrency(), 1);

/**
 * Owns the selected backend behind the public ThreadPool's opaque implementation.
 * Destroying the backend stops its workers before releasing its shared state.
 **/
class ThreadPool::Impl {
  public:
    /**
     * Creates a validated, initially stopped backend; the facade calls Start() afterward.
     * @param minimum Minimum builtin worker count, retained as configuration for Boost.
     * @param maximum Nonzero maximum worker count, no less than minimum.
     * @throws std::invalid_argument If the limits are invalid.
     **/
    explicit Impl(size_t minimum, size_t maximum) : backend(MakeThreadPoolBackend(minimum, maximum)) {}
    /**
     * Creates a stopped backend borrowing the caller's Boost.Asio context.
     * @param context Caller-owned event loop, kept alive for the facade's lifetime.
     * @param minimum Retained minimum worker setting.
     * @param maximum Nonzero limit on concurrent cpr tasks, no less than minimum.
     * @throws std::invalid_argument If the limits are invalid.
     * @throws std::logic_error If the builtin backend was selected.
     **/
    Impl(boost::asio::io_context& context, size_t minimum, size_t maximum) : backend(MakeThreadPoolBackend(context, minimum, maximum)) {}
    /** Stops work before releasing ownership, even when an external-context callback holds the state alive. */
    ~Impl() {
        backend->Stop();
        // Finish cancellation synchronously even if external callbacks still retain
        // backend state, including work queued by active tasks during Stop().
        // Destroy canceled task captures after releasing the mutex; their destructors may reenter the pool.
        // cppcheck-suppress variableScope
        std::queue<std::function<void()>> canceled;
        {
            const std::lock_guard lock(backend->mutex);
            backend->tasks.swap(canceled);
        }
    }
    /** Backend state shared only with active external-context callbacks to keep their accounting safe. */
    std::shared_ptr<ThreadPoolBackend> backend;
};

ThreadPool::ThreadPool(size_t minimum, size_t maximum) : impl(std::make_unique<Impl>(minimum, maximum)) {
    Start();
}
ThreadPool::ThreadPool(boost::asio::io_context& context, size_t minimum, size_t maximum) : impl(std::make_unique<Impl>(context, minimum, maximum)) {
    Start();
}
ThreadPool::~ThreadPool() = default;

ThreadPool::State ThreadPool::GetState() const {
    const std::lock_guard lock(impl->backend->mutex);
    return impl->backend->state;
}
size_t ThreadPool::GetMinThreadCount() const {
    const std::lock_guard lock(impl->backend->mutex);
    return impl->backend->minimum;
}
size_t ThreadPool::GetMaxThreadCount() const {
    const std::lock_guard lock(impl->backend->mutex);
    return impl->backend->maximum;
}
size_t ThreadPool::GetCurThreadCount() const {
    const std::lock_guard lock(impl->backend->mutex);
    return impl->backend->current;
}
size_t ThreadPool::GetIdleThreadCount() const {
    const std::lock_guard lock(impl->backend->mutex);
    return impl->backend->current - impl->backend->active;
}
void ThreadPool::SetMinThreadCount(size_t minimum) {
    impl->backend->Configure(&minimum, nullptr);
}
void ThreadPool::SetMaxThreadCount(size_t maximum) {
    impl->backend->Configure(nullptr, &maximum);
}
void ThreadPool::SetThreadCounts(size_t minimum, size_t maximum) {
    impl->backend->Configure(&minimum, &maximum);
}
void ThreadPool::Start() {
    impl->backend->Start();
}
void ThreadPool::Stop() {
    impl->backend->Stop();
}
void ThreadPool::Wait() const {
    impl->backend->Wait();
}
void ThreadPool::enqueue(std::function<void()> task) {
    impl->backend->Enqueue(std::move(task));
}
} // namespace cpr
