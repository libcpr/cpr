#ifndef CPR_PROXYAUTH_H
#define CPR_PROXYAUTH_H

#include <initializer_list>
#include <map>

#include "cpr/auth.h"
#include "cpr/util.h"

namespace cpr {
class EncodedAuthentication {
  public:
    EncodedAuthentication() : auth_string_{""} {}
    EncodedAuthentication(std::string username, std::string password) : auth_string_{cpr::util::urlEncode(std::move(username)) + ":" + cpr::util::urlEncode(std::move(password))} {}
    EncodedAuthentication(const EncodedAuthentication& other) = default;
    EncodedAuthentication(EncodedAuthentication&& old) noexcept = default;
    virtual ~EncodedAuthentication() noexcept;

    EncodedAuthentication& operator=(EncodedAuthentication&& old) noexcept = default;
    EncodedAuthentication& operator=(const EncodedAuthentication& other) = default;

    const char* GetAuthString() const noexcept;

  protected:
    std::string auth_string_;
};

class ProxyAuthentication {
  public:
    ProxyAuthentication() = default;
    ProxyAuthentication(const std::initializer_list<std::pair<const std::string, EncodedAuthentication>>& auths) : proxyAuth_{auths} {}
    ProxyAuthentication(const std::map<std::string, EncodedAuthentication>& auths) : proxyAuth_{auths} {}

    bool has(const std::string& protocol) const;
    const char* operator[](const std::string& protocol);

  private:
    std::map<std::string, EncodedAuthentication> proxyAuth_;
};

} // namespace cpr

#endif
