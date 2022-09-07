#include "cpr/proxyauth.h"
#include "cpr/util.h"

namespace cpr {
EncodedAuthentication::~EncodedAuthentication() noexcept {
    util::secureStringClear(auth_string_);
}

const char* EncodedAuthentication::GetAuthString() const noexcept {
    return auth_string_.c_str();
}

bool ProxyAuthentication::has(const std::string& protocol) const {
    return proxyAuth_.count(protocol) > 0;
}

const char* ProxyAuthentication::operator[](const std::string& protocol) {
    return proxyAuth_[protocol].GetAuthString();
}

} // namespace cpr
