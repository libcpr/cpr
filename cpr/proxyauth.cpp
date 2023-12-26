#include "cpr/proxyauth.h"
#include "cpr/util.h"
#include <string>

namespace cpr {
EncodedAuthentication::~EncodedAuthentication() noexcept {
    util::secureStringClear(username);
    util::secureStringClear(password);
}

const std::string& EncodedAuthentication::GetUsername() const {
    return username;
}

const std::string& EncodedAuthentication::GetPassword() const {
    return password;
}

bool ProxyAuthentication::has(const std::string& protocol) const {
    return proxyAuth_.count(protocol) > 0;
}

const char* ProxyAuthentication::GetUsername(const std::string& protocol) {
    return proxyAuth_[protocol].username.c_str();
}

const char* ProxyAuthentication::GetPassword(const std::string& protocol) {
    return proxyAuth_[protocol].password.c_str();
}

} // namespace cpr
