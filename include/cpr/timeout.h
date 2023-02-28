#ifndef CPR_TIMEOUT_H
#define CPR_TIMEOUT_H

#include <chrono>
#include <cstdint>

namespace cpr {

class Timeout {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Timeout(const std::chrono::milliseconds& duration) : ms{duration} {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Timeout(const std::int32_t& milliseconds) : Timeout{std::chrono::milliseconds(milliseconds)} {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Timeout(const std::chrono::seconds& duration) : ms{std::chrono::milliseconds(duration).count()} {}

    // No way around since curl uses a long here.
    // NOLINTNEXTLINE(google-runtime-int)
    long Milliseconds() const;

    std::chrono::milliseconds ms;
};

} // namespace cpr

#endif
