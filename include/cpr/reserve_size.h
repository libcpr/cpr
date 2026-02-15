#ifndef CPR_RESERVE_SIZE_H
#define CPR_RESERVE_SIZE_H

#include <cstdint>

namespace cpr {

class ReserveSize {
  public:
    ReserveSize(const std::size_t _size) : size(_size) {}

    std::size_t size = 0;
};

} // namespace cpr

#endif
