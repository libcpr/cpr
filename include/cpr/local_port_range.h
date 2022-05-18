#ifndef CPR_LOCAL_PORT_RANGE_H
#define CPR_LOCAL_PORT_RANGE_H

#include <cstdint>

namespace cpr {

class LocalPortRange {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor)
    LocalPortRange(const std::uint16_t p_localportrange) : localportrange_(p_localportrange) {}

    operator std::uint16_t() const {
        return localportrange_;
    }

  private:
    std::uint16_t localportrange_ = 0;
};

} // namespace cpr

#endif