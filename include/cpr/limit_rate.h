#ifndef CPR_SPEED_LIMIT_H
#define CPR_SPEED_LIMIT_H

#include <cstdint>

namespace cpr {

class LimitRate {
  public:
    LimitRate(const std::int64_t downrate, const std::int64_t uprate) : downrate(downrate), uprate(uprate) {}

    std::int64_t downrate = 0;
    std::int64_t uprate = 0;
};

} // namespace cpr

#endif