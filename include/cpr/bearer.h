#ifndef CPR_BEARER_H
#define CPR_BEARER_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <string>
#include <utility>
#endif

#include "cpr/util.h"
#include <curl/curlver.h>

namespace cpr {

// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
EXPORT_CPR class Bearer {
  public:
    Bearer(std::string_view token) : token_string_{token} {}
    Bearer(const Bearer& other) = default;
    Bearer(Bearer&& old) noexcept = default;
    virtual ~Bearer() noexcept = default;

    Bearer& operator=(Bearer&& old) noexcept = default;
    Bearer& operator=(const Bearer& other) = default;

    [[nodiscard]] virtual const char* GetToken() const noexcept {
        return token_string_.c_str();
    }

  protected:
    util::SecureString token_string_;
};
#endif

} // namespace cpr

#endif
