#include "cpr/auth.h"

namespace cpr {

const char* Authentication::GetAuthString() const noexcept {
    return auth_string_.data();
}

bool ProxyAuthentication::has(const std::string& protocol) const {
    return proxyAuth_.count(protocol) > 0;
}

const char* ProxyAuthentication::operator[](const std::string& protocol) {
    return proxyAuth_[protocol].GetAuthString();
}

} // namespace cpr
