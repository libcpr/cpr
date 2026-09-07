#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <string_view>
#endif

#include "cpr/util.h"
#include <cstdint>

namespace cpr {

EXPORT_CPR enum class AuthMode : uint8_t { BASIC, DIGEST, NTLM, NEGOTIATE, ANY, ANYSAFE };

EXPORT_CPR class Authentication {
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
