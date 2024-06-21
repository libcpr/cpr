#include "cpr/auth.h"
#include "cpr/util.h"

#include <string_view>
#include <utility>

namespace cpr {

Authentication::Authentication(std::string_view username, std::string_view password, AuthMode auth_mode) : auth_mode_{auth_mode} {
    auth_string_.reserve(username.size() + 1 + password.size());
    auth_string_ += username;
    auth_string_ += ':';
    auth_string_ += password;
}

Authentication::Authentication(Authentication&& old) noexcept : auth_string_{std::move(old.auth_string_)}, auth_mode_{old.auth_mode_} {
    old.auth_string_.resize(old.auth_string_.capacity());
}

Authentication& Authentication::operator=(Authentication&& old) noexcept {
    auth_mode_ = old.auth_mode_;
    util::secureStringClear(auth_string_);
    auth_string_ = std::move(old.auth_string_);
    old.auth_string_.resize(old.auth_string_.capacity());
    return *this;
}

Authentication::~Authentication() noexcept {
    util::secureStringClear(auth_string_);
}

const char* Authentication::GetAuthString() const noexcept {
    return auth_string_.c_str();
}

AuthMode Authentication::GetAuthMode() const noexcept {
    return auth_mode_;
}
} // namespace cpr
