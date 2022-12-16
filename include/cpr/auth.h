#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>

#include <utility>

namespace cpr {

enum class AuthMode { BASIC, DIGEST, NTLM };

class Authentication {
  public:
    Authentication(std::string username, std::string password, AuthMode auth_mode) : auth_string_{std::move(username) + ":" + std::move(password)}, auth_mode_{std::move(auth_mode)} {}
    Authentication(const Authentication& other) = default;
    Authentication(Authentication&& old) noexcept = default;
    ~Authentication() noexcept;

    Authentication& operator=(Authentication&& old) noexcept = default;
    Authentication& operator=(const Authentication& other) = default;

    const char* GetAuthString() const noexcept;
    AuthMode GetAuthMode() const noexcept;

  private:
    std::string auth_string_;
    AuthMode auth_mode_;
};

} // namespace cpr

#endif
