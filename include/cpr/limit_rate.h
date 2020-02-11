#ifndef CPR_SPEED_LIMIT_H
#define CPR_SPEED_LIMIT_H

#include <cstdint>

namespace cpr {

class LimitRate {
  public:
    LimitRate(const std::int64_t receive_rate, const std::int64_t send_rate) 
      : receive_rate(receive_rate), send_rate(send_rate) {}

    std::int64_t receive_rate = 0;
    std::int64_t send_rate = 0;
};

} // namespace cpr

#endif
