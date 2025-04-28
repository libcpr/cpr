#ifndef CPR_LOW_SPEED_H
#define CPR_LOW_SPEED_H

#include <cstdint>
#include <chrono>

namespace cpr {

class LowSpeed {
  public:

    [[deprecated("Will be removed in CPR 2.x - Use the constructor with std::chrono::seconds instead of std::int32_t")]]
    LowSpeed(const std::int32_t p_limit, const std::int32_t p_time) : limit(p_limit), time(std::chrono::seconds(p_time)) {}

    LowSpeed(const std::int32_t p_limit, const std::chrono::seconds p_time) : limit(p_limit), time(p_time) {}

    std::int32_t limit;
    std::chrono::seconds time;
};

} // namespace cpr

#endif
