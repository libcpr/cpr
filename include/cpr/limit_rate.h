#ifndef CPR_SPEED_LIMIT_H
#define CPR_SPEED_LIMIT_H

#include <cstdint>

namespace cpr {

class LimitRate {
  public:
    LimitRate(const std::int64_t rate) : rate(rate) {}

    std::int64_t rate = 0;
};

} // namespace cpr

#endif
