#ifndef CPR_UTIL_H
#define CPR_UTIL_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "cpr/callback.h"
#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/secure_string.h"

namespace cpr::util {

Header parseHeader(const std::string& headers, std::string* status_line = nullptr, std::string* reason = nullptr);
Cookies parseCookies(curl_slist* raw_cookies);
size_t readUserFunction(char* ptr, size_t size, size_t nitems, const ReadCallback* read);
size_t headerUserFunction(char* ptr, size_t size, size_t nmemb, const HeaderCallback* header);
size_t writeFunction(char* ptr, size_t size, size_t nmemb, std::string* data);
size_t writeFileFunction(char* ptr, size_t size, size_t nmemb, std::ofstream* file);
size_t writeUserFunction(char* ptr, size_t size, size_t nmemb, const WriteCallback* write);

template <typename T = ProgressCallback>
int progressUserFunction(const T* progress, cpr_pf_arg_t dltotal, cpr_pf_arg_t dlnow, cpr_pf_arg_t ultotal, cpr_pf_arg_t ulnow) {
    const int cancel_retval{1};
#ifdef CURL_PROGRESSFUNC_CONTINUE // Not always defined. Ref: https://github.com/libcpr/cpr/issues/932
    static_assert(cancel_retval != CURL_PROGRESSFUNC_CONTINUE);
#endif // CURL_PROGRESSFUNC_CONTINUE
    return (*progress)(dltotal, dlnow, ultotal, ulnow) ? 0 : cancel_retval;
}
int debugUserFunction(CURL* handle, curl_infotype type, char* data, size_t size, const DebugCallback* debug);
std::vector<std::string> split(const std::string& to_split, char delimiter);
util::SecureString urlEncode(std::string_view s);
util::SecureString urlDecode(std::string_view s);

bool isTrue(const std::string& s);

/**
 * Parses the given std::string into time_t (unix ms).
 * This parsing happens time_t size agnostic since time_t does not use the same underlying type on all systems/compilers.
 **/
time_t sTimestampToT(const std::string&);

} // namespace cpr::util

#endif
