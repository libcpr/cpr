#include "cpr/auth.h"

namespace cpr {
const char* Authentication::GetAuthString() const noexcept {
    return auth_string_.c_str();
}

AuthMode Authentication::GetAuthMode() const noexcept {
    return auth_mode_;
}
} // namespace cpr
