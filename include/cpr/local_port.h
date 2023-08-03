#ifndef CPR_LOCAL_PORT_H
#define CPR_LOCAL_PORT_H

#include <cstdint>

namespace cpr {

class LocalPort {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor)
    LocalPort(const std::uint16_t p_localport) : localport_(p_localport) {}

    operator std::uint16_t() const {
        return localport_;
    }

  private:
    std::uint16_t localport_ = 0;
};

} // namespace cpr

#endif
