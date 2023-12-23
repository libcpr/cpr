#include "cpr/timeout.h"

#include <chrono>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace cpr {

// No way around since curl uses a long here.
// NOLINTNEXTLINE(google-runtime-int)
long Timeout::Milliseconds() const {
    static_assert(std::is_same_v<std::chrono::milliseconds, decltype(ms)>, "Following casting expects milliseconds.");

    // No way around since curl uses a long here.
    // NOLINTNEXTLINE(google-runtime-int)
    if (ms.count() > static_cast<std::chrono::milliseconds::rep>(std::numeric_limits<long>::max())) {
        throw std::overflow_error("cpr::Timeout: timeout value overflow: " + std::to_string(ms.count()) + " ms.");
    }
    // No way around since curl uses a long here.
    // NOLINTNEXTLINE(google-runtime-int)
    if (ms.count() < static_cast<std::chrono::milliseconds::rep>(std::numeric_limits<long>::min())) {
        throw std::underflow_error("cpr::Timeout: timeout value underflow: " + std::to_string(ms.count()) + " ms.");
    }

    // No way around since curl uses a long here.
    // NOLINTNEXTLINE(google-runtime-int)
    return static_cast<long>(ms.count());
}

} // namespace cpr
