#ifndef CPR_PROXYAUTH_H
#define CPR_PROXYAUTH_H

#include <initializer_list>
#include <map>

#include "cpr/auth.h"
#include "cpr/util.h"

namespace cpr {
class EncodedAuthentication {
  public:
    EncodedAuthentication() : username{""}, password{""} {}
    EncodedAuthentication(std::string username_, std::string password_) : username{cpr::util::urlEncode(username_)}, password{cpr::util::urlEncode(password_)} {}
    EncodedAuthentication(const EncodedAuthentication& other) = default;
    EncodedAuthentication(EncodedAuthentication&& old) noexcept = default;
    virtual ~EncodedAuthentication() noexcept;

    EncodedAuthentication& operator=(EncodedAuthentication&& old) noexcept = default;
    EncodedAuthentication& operator=(const EncodedAuthentication& other) = default;

    std::string username, password;
};

class ProxyAuthentication {
  public:
    ProxyAuthentication() = default;
    ProxyAuthentication(const std::initializer_list<std::pair<const std::string, EncodedAuthentication>>& auths) : proxyAuth_{auths} {}
    ProxyAuthentication(const std::map<std::string, EncodedAuthentication>& auths) : proxyAuth_{auths} {}

    bool has(const std::string& protocol) const;
    const char* GetUsername(const std::string& protocol);
    const char* GetPassword(const std::string& protocol);

  private:
    std::map<std::string, EncodedAuthentication> proxyAuth_;
};

} // namespace cpr

#endif
