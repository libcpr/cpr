#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>
#include <string_view>

namespace cpr {

enum class AuthMode { BASIC, DIGEST, NTLM, NEGOTIATE };

class Authentication {
  public:
    Authentication(std::string_view username, std::string_view password, AuthMode auth_mode);
    Authentication(const Authentication& other) = default;
    ~Authentication() noexcept;

    Authentication& operator=(const Authentication& other) = default;

    const char* GetAuthString() const noexcept;
    AuthMode GetAuthMode() const noexcept;

  private:
    std::string auth_string_;
    AuthMode auth_mode_;
};

} // namespace cpr

#endif
