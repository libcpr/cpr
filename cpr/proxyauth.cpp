#include "cpr/proxyauth.h"
#include "cpr/util.h"

namespace cpr {
EncodedAuthentication::~EncodedAuthentication() noexcept {
    util::secureStringClear(username);
    util::secureStringClear(password);
}

bool ProxyAuthentication::has(const std::string& protocol) const {
    return proxyAuth_.count(protocol) > 0;
}

const char* ProxyAuthentication::GetUsername(const std::string& protocol) {
    return proxyAuth_[protocol].username.c_str( );
}

const char* ProxyAuthentication::GetPassword(const std::string& protocol) {
    return proxyAuth_[protocol].username.c_str();
}

} // namespace cpr
