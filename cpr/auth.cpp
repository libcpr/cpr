#include "cpr/auth.h"

namespace cpr {
const char* Authentication::GetAuthString() const noexcept {
    return auth_string_.c_str();
}
} // namespace cpr
