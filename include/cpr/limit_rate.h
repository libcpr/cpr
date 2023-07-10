#ifndef CPR_SPEED_LIMIT_H
#define CPR_SPEED_LIMIT_H

#include <cstdint>

namespace cpr {

class LimitRate {
  public:
    LimitRate(const std::int64_t p_downrate, const std::int64_t p_uprate) : downrate(p_downrate), uprate(p_uprate) {}

    std::int64_t downrate = 0;
    std::int64_t uprate = 0;
};

} // namespace cpr

#endif
