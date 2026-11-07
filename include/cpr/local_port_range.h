#ifndef CPR_LOCAL_PORT_RANGE_H
#define CPR_LOCAL_PORT_RANGE_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <cstdint>
#endif

namespace cpr {

EXPORT_CPR class LocalPortRange {
  public:
    LocalPortRange(const std::uint16_t p_localportrange) : localportrange_(p_localportrange) {}

    operator std::uint16_t() const {
        return localportrange_;
    }

  private:
    std::uint16_t localportrange_ = 0;
};

} // namespace cpr

#endif
