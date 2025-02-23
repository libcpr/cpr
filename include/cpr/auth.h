#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>
#include <string_view>

#include "cpr/util.h"

namespace cpr {

enum class AuthMode { BASIC, DIGEST, NTLM, NEGOTIATE, ANY, ANYSAFE };

class Authentication {
  public:
    Authentication(std::string_view username, std::string_view password, AuthMode auth_mode);

    const char* GetAuthString() const noexcept;
    AuthMode GetAuthMode() const noexcept;

  private:
    util::SecureString auth_string_;
    AuthMode auth_mode_;
};

} // namespace cpr

#endif
