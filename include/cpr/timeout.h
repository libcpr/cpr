#ifndef CPR_TIMEOUT_H
#define CPR_TIMEOUT_H

#include <cstdint>

namespace cpr {

class Timeout {
  public:
    Timeout(const std::int32_t& timeout) : ms(timeout) {}

    std::int32_t ms;
};

} // namespace cpr

#endif
