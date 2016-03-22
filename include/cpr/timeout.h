#ifndef CPR_TIMEOUT_H
#define CPR_TIMEOUT_H

#include <cstdint>
#include <chrono>
#include <limits>
#include <stdexcept>

namespace cpr {

class Timeout {
  public:
    Timeout(const std::chrono::milliseconds& duration) : ms{duration} {
        if(ms.count() > std::numeric_limits<long>::max()) {
            throw std::overflow_error("cpr::Timeout: timeout value overflow.");
        }
        if(ms.count() < std::numeric_limits<long>::min()) {
            throw std::underflow_error("cpr::Timeout: timeout value underflow.");
        }
    }
    Timeout(const std::int32_t& milliseconds)
            : Timeout{std::chrono::milliseconds(milliseconds)} {}

    std::chrono::milliseconds ms;
};

} // namespace cpr

#endif
