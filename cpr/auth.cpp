#include "cpr/auth.h"
#include "cpr/util.h"

namespace cpr {
Authentication::~Authentication() noexcept {
    util::secureStringClear(auth_string_);
}

const char* Authentication::GetAuthString() const noexcept {
    return auth_string_.c_str();
}
} // namespace cpr
