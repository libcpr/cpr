#ifndef CPR_BEARER_H
#define CPR_BEARER_H

#include <curl/curlver.h>
#include <string>

#include <utility>

#include "cpr/util.h"

namespace cpr {

// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
class Bearer {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Bearer(std::string_view token) : token_string_{token} {}
    Bearer(const Bearer& other) = default;
    Bearer(Bearer&& old) noexcept = default;
    virtual ~Bearer() noexcept = default;

    Bearer& operator=(Bearer&& old) noexcept = default;
    Bearer& operator=(const Bearer& other) = default;

    virtual const char* GetToken() const noexcept {
        return token_string_.c_str();
    }

  protected:
    util::SecureString token_string_;
};
#endif

} // namespace cpr

#endif
