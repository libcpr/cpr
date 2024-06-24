#include "cpr/auth.h"
#include "cpr/util.h"

#include <string_view>

namespace cpr {

Authentication::Authentication(std::string_view username, std::string_view password, AuthMode auth_mode) : auth_mode_{auth_mode} {
    auth_string_.reserve(username.size() + 1 + password.size() + 1);
    auth_string_.insert(auth_string_.end(), username.begin(), username.end());
    auth_string_.push_back(':');
    auth_string_.insert(auth_string_.end(), password.begin(), password.end());
    auth_string_.push_back('\0');
}

Authentication::~Authentication() noexcept {
    util::secureClear(auth_string_.data(), auth_string_.size());
}

const char* Authentication::GetAuthString() const noexcept {
    return auth_string_.data();
}

AuthMode Authentication::GetAuthMode() const noexcept {
    return auth_mode_;
}
} // namespace cpr
