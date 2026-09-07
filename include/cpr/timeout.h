#ifndef CPR_TIMEOUT_H
#define CPR_TIMEOUT_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <chrono>
#endif

#include <cstdint>

namespace cpr {

EXPORT_CPR class Timeout {
  public:
    // Template constructor to accept any chrono duration type and convert it to milliseconds
    template <typename Rep, typename Period>
    Timeout(const std::chrono::duration<Rep, Period>& duration) : ms{std::chrono::duration_cast<std::chrono::milliseconds>(duration)} {}

    Timeout(const std::int32_t& milliseconds) : Timeout{std::chrono::milliseconds(milliseconds)} {}

    // No way around since curl uses a long here.
    // NOLINTNEXTLINE(google-runtime-int)
    [[nodiscard]]
    long Milliseconds() const;

    std::chrono::milliseconds ms;
};

} // namespace cpr

#endif
