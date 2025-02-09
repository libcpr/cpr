#include "cpr/proxyauth.h"
#include <string>
#include <string_view>

namespace cpr {

std::string_view EncodedAuthentication::GetUsername() const {
    return username;
}

std::string_view EncodedAuthentication::GetPassword() const {
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

} // namespace cpr
