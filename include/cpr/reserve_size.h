#ifndef CPR_RESERVE_SIZE_H
#define CPR_RESERVE_SIZE_H

#include "cpr/export.h"

#include <cstdint>

namespace cpr {

EXPORT_CPR class ReserveSize {
  public:
    ReserveSize(const std::size_t _size) : size(_size) {}

    std::size_t size = 0;
};

} // namespace cpr

#endif
