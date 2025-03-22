#ifndef CPR_ASYNC_WRAPPER_H
#define CPR_ASYNC_WRAPPER_H

#include <atomic>
#include <future>
#include <memory>

#include "cpr/response.h"

namespace cpr {
enum class [[nodiscard]] CancellationResult { failure, success, invalid_operation };

/**
 * A class template intended to wrap results of async operations (instances of std::future<T>)
 * and also provide extended capablilities relaed to these requests, for example cancellation.
 *
 * The RAII semantics are the same as std::future<T> - moveable, not copyable.
 */
template <typename T, bool isCancellable = false>
class AsyncWrapper {
  private:
    std::future<T> future;
    std::shared_ptr<std::atomic_bool> is_cancelled;

  public:
    // Constructors
    explicit AsyncWrapper(std::future<T>&& f) : future{std::move(f)} {}
    AsyncWrapper(std::future<T>&& f, std::shared_ptr<std::atomic_bool>&& cancelledState) : future{std::move(f)}, is_cancelled{std::move(cancelledState)} {}

    // Copy Semantics
    AsyncWrapper(const AsyncWrapper&) = delete;
    AsyncWrapper& operator=(const AsyncWrapper&) = delete;

    // Move Semantics
    AsyncWrapper(AsyncWrapper&&) noexcept = default;
    AsyncWrapper& operator=(AsyncWrapper&&) noexcept = default;

    // Destructor
    ~AsyncWrapper() {
        if constexpr (isCancellable) {
            if (is_cancelled) {
                is_cancelled->store(true);
            }
        }
    }
    // These methods replicate the behaviour of std::future<T>
    [[nodiscard]] T get() {
        if constexpr (isCancellable) {
            if (IsCancelled()) {
                throw std::logic_error{"Calling AsyncWrapper::get on a cancelled request!"};
            }
        }
        if (!future.valid()) {
            throw std::logic_error{"Calling AsyncWrapper::get when the associated future instance is invalid!"};
        }
        return future.get();
    }

    [[nodiscard]] bool valid() const noexcept {
        if constexpr (isCancellable) {
            return !is_cancelled->load() && future.valid();
        } else {
            return future.valid();
        }
    }

    void wait() const {
        if constexpr (isCancellable) {
            if (is_cancelled->load()) {
                throw std::logic_error{"Calling AsyncWrapper::wait when the associated future is invalid or cancelled!"};
            }
        }
        if (!future.valid()) {
            throw std::logic_error{"Calling AsyncWrapper::wait_until when the associated future is invalid!"};
        }
        future.wait();
    }

    template <class Rep, class Period>
    std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeout_duration) const {
        if constexpr (isCancellable) {
            if (IsCancelled()) {
                throw std::logic_error{"Calling AsyncWrapper::wait_for when the associated future is cancelled!"};
            }
        }
        if (!future.valid()) {
            throw std::logic_error{"Calling AsyncWrapper::wait_until when the associated future is invalid!"};
        }
        return future.wait_for(timeout_duration);
    }

    template <class Clock, class Duration>
    std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) const {
        if constexpr (isCancellable) {
            if (IsCancelled()) {
                throw std::logic_error{"Calling AsyncWrapper::wait_until when the associated future is cancelled!"};
            }
        }
        if (!future.valid()) {
            throw std::logic_error{"Calling AsyncWrapper::wait_until when the associated future is invalid!"};
        }
        return future.wait_until(timeout_time);
    }

    std::shared_future<T> share() noexcept {
        return future.share();
    }

    // Cancellation-related methods
    CancellationResult Cancel() {
        if constexpr (!isCancellable) {
            return CancellationResult::invalid_operation;
        }
        if (!future.valid() || is_cancelled->load()) {
            return CancellationResult::invalid_operation;
        }
        is_cancelled->store(true);
        return CancellationResult::success;
    }

    [[nodiscard]] bool IsCancelled() const {
        if constexpr (isCancellable) {
            return is_cancelled->load();
        } else {
            return false;
        }
    }
};

// Deduction guides
template <typename T>
AsyncWrapper(std::future<T>&&) -> AsyncWrapper<T, false>;

template <typename T>
AsyncWrapper(std::future<T>&&, std::shared_ptr<std::atomic_bool>&&) -> AsyncWrapper<T, true>;

} // namespace cpr


#endif
