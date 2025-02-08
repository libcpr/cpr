#include "cpr/auth.h"

#include <string_view>

namespace cpr {

Authentication::Authentication(std::string_view username, std::string_view password, AuthMode auth_mode) : auth_mode_{auth_mode} {
    auth_string_.reserve(username.size() + 1 + password.size());
    auth_string_ += username;
    auth_string_ += ':';
    auth_string_ += password;
}

const char* Authentication::GetAuthString() const noexcept {
    return auth_string_.c_str();
}

AuthMode Authentication::GetAuthMode() const noexcept {
    return auth_mode_;
}
} // namespace cpr
