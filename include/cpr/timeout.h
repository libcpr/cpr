#ifndef CPR_TIMEOUT_H
#define CPR_TIMEOUT_H

#include <cstdint>
#include <chrono>

namespace cpr {

class Timeout {
  public:
    Timeout(const std::chrono::milliseconds& timeout) : ms(timeout) {}
    Timeout(const std::int32_t& timeout) : ms(timeout) {}

    std::chrono::milliseconds ms;
};

} // namespace cpr

#endif
