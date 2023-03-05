#ifndef CPR_PROXYAUTH_H
#define CPR_PROXYAUTH_H

#include <initializer_list>
#include <map>
#include <string>

#include "cpr/auth.h"
#include "cpr/util.h"

namespace cpr {
class ProxyAuthentication;

class EncodedAuthentication {
    friend ProxyAuthentication;

  public:
    EncodedAuthentication() = default;
    EncodedAuthentication(const std::string& p_username, const std::string& p_password) : username(util::urlEncode(p_username)), password(util::urlEncode(p_password)) {}
    EncodedAuthentication(const EncodedAuthentication& other) = default;
    EncodedAuthentication(EncodedAuthentication&& old) noexcept = default;
    virtual ~EncodedAuthentication() noexcept;

    EncodedAuthentication& operator=(EncodedAuthentication&& old) noexcept = default;
    EncodedAuthentication& operator=(const EncodedAuthentication& other) = default;

    [[nodiscard]] const std::string& GetUsername() const;
    [[nodiscard]] const std::string& GetPassword() const;

  private:
    std::string username;
    std::string password;
};

class ProxyAuthentication {
  public:
    ProxyAuthentication() = default;
    ProxyAuthentication(const std::initializer_list<std::pair<const std::string, EncodedAuthentication>>& auths) : proxyAuth_{auths} {}
    explicit ProxyAuthentication(const std::map<std::string, EncodedAuthentication>& auths) : proxyAuth_{auths} {}

    [[nodiscard]] bool has(const std::string& protocol) const;
    const char* GetUsername(const std::string& protocol);
    const char* GetPassword(const std::string& protocol);

  private:
    std::map<std::string, EncodedAuthentication> proxyAuth_;
};

} // namespace cpr

#endif
