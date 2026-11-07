#ifndef CPR_LIMIT_RATE_H
#define CPR_LIMIT_RATE_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <cstdint>
#endif

namespace cpr {

EXPORT_CPR class LimitRate {
  public:
    LimitRate(const std::int64_t p_downrate, const std::int64_t p_uprate) : downrate(p_downrate), uprate(p_uprate) {}

    std::int64_t downrate = 0;
    std::int64_t uprate = 0;
};

} // namespace cpr

#endif
