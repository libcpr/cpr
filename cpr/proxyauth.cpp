#include "cpr/proxyauth.h"
#include "cpr/secure_string.h"
#include <string>
#include <string_view>

namespace cpr {

std::string_view EncodedAuthentication::GetUsername() const {
    return username;
}

std::string_view EncodedAuthentication::GetPassword() const {
    return password;
}

const util::SecureString& EncodedAuthentication::GetUsernameUnderlying() const {
    return username;
}

const util::SecureString& EncodedAuthentication::GetPasswordUnderlying() const {
    return password;
}

bool ProxyAuthentication::has(const std::string& protocol) const {
    return proxyAuth_.count(protocol) > 0;
}

std::string_view ProxyAuthentication::GetUsername(const std::string& protocol) {
    return proxyAuth_[protocol].GetUsername();
}

std::string_view ProxyAuthentication::GetPassword(const std::string& protocol) {
    return proxyAuth_[protocol].GetPassword();
}

const util::SecureString& ProxyAuthentication::GetUsernameUnderlying(const std::string& protocol) const {
    return proxyAuth_.at(protocol).GetUsernameUnderlying();
}

const util::SecureString& ProxyAuthentication::GetPasswordUnderlying(const std::string& protocol) const {
    return proxyAuth_.at(protocol).GetPasswordUnderlying();
}

} // namespace cpr
