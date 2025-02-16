#ifndef CPR_PROXYAUTH_H
#define CPR_PROXYAUTH_H

#include <initializer_list>
#include <map>
#include <string>
#include <string_view>

#include "cpr/auth.h"
#include "cpr/util.h"

namespace cpr {
class ProxyAuthentication;

class EncodedAuthentication {
    friend ProxyAuthentication;

  public:
    EncodedAuthentication() = default;
    EncodedAuthentication(std::string_view p_username, std::string_view p_password) : username(util::urlEncode(p_username)), password(util::urlEncode(p_password)) {}
    EncodedAuthentication(const EncodedAuthentication& other) = default;
    EncodedAuthentication(EncodedAuthentication&& old) noexcept = default;
    virtual ~EncodedAuthentication() noexcept = default;

    EncodedAuthentication& operator=(EncodedAuthentication&& old) noexcept = default;
    EncodedAuthentication& operator=(const EncodedAuthentication& other) = default;

    [[nodiscard]] std::string_view GetUsername() const;
    [[nodiscard]] std::string_view GetPassword() const;
    [[nodiscard]] const util::SecureString& GetUsernameUnderlying() const;
    [[nodiscard]] const util::SecureString& GetPasswordUnderlying() const;

  private:
    util::SecureString username;
    util::SecureString password;
};

class ProxyAuthentication {
  public:
    ProxyAuthentication() = default;
    ProxyAuthentication(const std::initializer_list<std::pair<const std::string, EncodedAuthentication>>& auths) : proxyAuth_{auths} {}
    explicit ProxyAuthentication(const std::map<std::string, EncodedAuthentication>& auths) : proxyAuth_{auths} {}

    [[nodiscard]] bool has(const std::string& protocol) const;
    [[nodiscard]] std::string_view GetUsername(const std::string& protocol);
    [[nodiscard]] std::string_view GetPassword(const std::string& protocol);
    [[nodiscard]] const util::SecureString& GetUsernameUnderlying(const std::string& protocol) const;
    [[nodiscard]] const util::SecureString& GetPasswordUnderlying(const std::string& protocol) const;

  private:
    std::map<std::string, EncodedAuthentication> proxyAuth_;
};

} // namespace cpr

#endif
