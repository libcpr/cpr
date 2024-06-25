#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string_view>
#include <vector>

namespace cpr {

enum class AuthMode { BASIC, DIGEST, NTLM };

class Authentication {
  public:
    Authentication(std::string_view username, std::string_view password, AuthMode auth_mode);
    Authentication(const Authentication& other) = default;
    Authentication(Authentication&& old) noexcept = default;
    ~Authentication() noexcept;

    Authentication& operator=(Authentication&& old) noexcept = default;
    Authentication& operator=(const Authentication& other) = default;

    const char* GetAuthString() const noexcept;
    AuthMode GetAuthMode() const noexcept;

  private:
    std::vector<char> auth_string_; // includes null terminator
    AuthMode auth_mode_;
};

} // namespace cpr

#endif
