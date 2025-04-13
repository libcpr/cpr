#ifndef CPR_ASYNC_WRAPPER_H
#define CPR_ASYNC_WRAPPER_H

#include <atomic>
#include <future>
#include <memory>

namespace cpr {
enum class [[nodiscard]] CancellationResult : uint8_t { failure, success, invalid_operation };

/**
 * A class template intended to wrap results of async operations (instances of std::future<T>)
 * and also provide extended capablilities relaed to these requests, for example cancellation.
 *
 * The RAII semantics are the same as std::future<T> - moveable, not copyable.
 */
template <typename T, bool isCancellable = false>
class AsyncWrapper;

template <typename T>
class AsyncWrapper<T, false> {
  private:
    friend class AsyncWrapper<T, true>;
    std::future<T> future;

    void throw_if_invalid(const char* error) const {
        if (!future.valid()) {
            throw std::logic_error{error};
        }
    }

  public:
    // Constructors
    AsyncWrapper() = default;
    explicit AsyncWrapper(std::future<T>&& f) : future{std::move(f)} {}

    // Copy Semantics
    AsyncWrapper(const AsyncWrapper&) = delete;
    AsyncWrapper& operator=(const AsyncWrapper&) = delete;

    // Move Semantics
    AsyncWrapper(AsyncWrapper&&) noexcept = default;
    AsyncWrapper& operator=(AsyncWrapper&&) noexcept = default;

    // Destructor
    ~AsyncWrapper() = default;

    // These methods replicate the behaviour of std::future<T>
    [[nodiscard]] T get() {
        throw_if_invalid("Calling AsyncWrapper::get when the associated future instance is invalid!");
        return future.get();
    }

    [[nodiscard]] bool valid() const noexcept {
        return future.valid();
    }

    void wait() const {
        throw_if_invalid("Calling AsyncWrapper::wait when the associated future is invalid!");
        future.wait();
    }

    template <class Rep, class Period>
    std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeout_duration) const {
        throw_if_invalid("Calling AsyncWrapper::wait_for when the associated future is invalid!");
        return future.wait_for(timeout_duration);
    }

    template <class Clock, class Duration>
    std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) const {
        throw_if_invalid("Calling AsyncWrapper::wait_until when the associated future is invalid!");
        return future.wait_until(timeout_time);
    }

    std::shared_future<T> share() noexcept {
        return future.share();
    }
};

template <typename T>
class AsyncWrapper<T, true> : public AsyncWrapper<T, false> {
  private:
    using base = AsyncWrapper<T, false>;
    std::shared_ptr<std::atomic_bool> is_cancelled;

    void throw_if_cancelled(const char* error) const {
        if (is_cancelled->load()) {
            throw std::logic_error{error};
        }
    }

  public:
    // Constructors
    AsyncWrapper(std::future<T>&& f, std::shared_ptr<std::atomic_bool>&& cancelledState) : base{std::move(f)}, is_cancelled{std::move(cancelledState)} {}

    // Copy Semantics
    AsyncWrapper(const AsyncWrapper&) = delete;
    AsyncWrapper& operator=(const AsyncWrapper&) = delete;

    // Move Semantics
    AsyncWrapper(AsyncWrapper&&) noexcept = default;
    AsyncWrapper& operator=(AsyncWrapper&&) noexcept = default;

    // Destructor
    ~AsyncWrapper() {
        if (is_cancelled) {
            is_cancelled->store(true);
        }
    }

    [[nodiscard]] T get() {
        throw_if_cancelled("Calling AsyncWrapper::get on a cancelled request!");
        return base::get();
    }

    [[nodiscard]] bool valid() const noexcept {
        return !is_cancelled->load() && base::future.valid();
    }

    void wait() const {
        throw_if_cancelled("Calling AsyncWrapper::wait when the associated future is invalid or cancelled!");
        base::wait();
    }

    template <class Rep, class Period>
    std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeout_duration) const {
        throw_if_cancelled("Calling AsyncWrapper::wait_for when the associated future is cancelled!");
        return base::wait_for(timeout_duration);
    }

    template <class Clock, class Duration>
    std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) const {
        throw_if_cancelled("Calling AsyncWrapper::wait_until when the associated future is cancelled!");
        return base::wait_until(timeout_time);
    }

    // Cancellation-related methods
    CancellationResult Cancel() {
        if (!base::future.valid() || is_cancelled->load()) {
            return CancellationResult::invalid_operation;
        }
        is_cancelled->store(true);
        return CancellationResult::success;
    }

    [[nodiscard]] bool IsCancelled() const {
        return is_cancelled->load();
    }
};

// Deduction guides
template <typename T>
AsyncWrapper(std::future<T>&&) -> AsyncWrapper<T, false>;

template <typename T>
AsyncWrapper(std::future<T>&&, std::shared_ptr<std::atomic_bool>&&) -> AsyncWrapper<T, true>;

} // namespace cpr


#endif
