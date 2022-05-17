#ifndef CPR_RESERVE_SIZE_H
#define CPR_RESERVE_SIZE_H

#include <cstdint>

namespace cpr {

class ReserveSize {
  public:
    ReserveSize(const size_t _size) : size(_size) {}

    size_t size = 0;
};

} // namespace cpr

#endif
