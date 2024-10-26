#ifndef CPR_TIMEOUT_H
#define CPR_TIMEOUT_H

#include <chrono>
#include <cstdint>

namespace cpr {

class Timeout {
  public:
    // Template constructor to accept any chrono duration type and convert it to milliseconds
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    template <typename Rep, typename Period>
    Timeout(const std::chrono::duration<Rep, Period>& duration)
            : ms{std::chrono::duration_cast<std::chrono::milliseconds>(duration)} {}

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Timeout(const std::int32_t& milliseconds) : Timeout{std::chrono::milliseconds(milliseconds)} {}

    // No way around since curl uses a long here.
    // NOLINTNEXTLINE(google-runtime-int)
    long Milliseconds() const;

    std::chrono::milliseconds ms;
};

} // namespace cpr

#endif
