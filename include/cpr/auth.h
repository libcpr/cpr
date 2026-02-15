#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <cstdint>
#include <string>
#include <string_view>

#include "cpr/util.h"

namespace cpr {

enum class AuthMode : uint8_t { BASIC, DIGEST, NTLM, NEGOTIATE, ANY, ANYSAFE };

class Authentication {
  public:
    Authentication(std::string_view username, std::string_view password, AuthMode auth_mode);

    [[nodiscard]] const char* GetAuthString() const noexcept;
    [[nodiscard]] AuthMode GetAuthMode() const noexcept;

  private:
    util::SecureString auth_string_;
    AuthMode auth_mode_;
};

} // namespace cpr

#endif
