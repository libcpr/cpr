#ifndef CPR_RESERVE_SIZE_H
#define CPR_RESERVE_SIZE_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <cstdint>
#endif

namespace cpr {

EXPORT_CPR class ReserveSize {
  public:
    ReserveSize(const std::size_t _size) : size(_size) {}

    std::size_t size = 0;
};

} // namespace cpr

#endif
