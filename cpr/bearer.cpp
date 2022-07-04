#include "cpr/bearer.h"
#include "cpr/util.h"

namespace cpr {
// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
Bearer::~Bearer() noexcept {
    util::secureStringClear(token_string_);
}

const char* Bearer::GetToken() const noexcept {
    return token_string_.c_str();
}
#endif
} // namespace cpr
