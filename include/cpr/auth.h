#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <initializer_list>
#include <map>
#include <string>

#include "cpr/defines.h"

namespace cpr {

class Authentication {
  public:
    Authentication() {}
    template <typename UserType, typename PassType>
    Authentication(UserType&& username, PassType&& password)
            : username_{CPR_FWD(username)}, password_{CPR_FWD(password)},
              auth_string_{username_ + ":" + password_} {}

    const char* GetAuthString() const noexcept;

  private:
    std::string username_;
    std::string password_;
    std::string auth_string_;
};

class ProxyAuthentication {
  public:
    ProxyAuthentication() {}
    ProxyAuthentication(const std::initializer_list<std::pair<const std::string,
                                                              Authentication>>& auths) :
    proxyAuth_{auths} {}

    bool has(const std::string& protocol) const;
    const char* operator[](const std::string& protocol);

private:
    std::map<std::string, Authentication> proxyAuth_;
};

} // namespace cpr

#endif
