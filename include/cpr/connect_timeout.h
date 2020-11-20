#ifndef CPR_CONNECT_TIMEOUT_H
#define CPR_CONNECT_TIMEOUT_H

#include "cpr/timeout.h"

namespace cpr {

class ConnectTimeout : public Timeout {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    ConnectTimeout(const std::chrono::milliseconds& duration) : Timeout{duration} {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    ConnectTimeout(const std::int32_t& milliseconds) : Timeout{milliseconds} {}
};

} // namespace cpr

#endif
