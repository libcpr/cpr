#ifndef CPR_UTIL_H
#define CPR_UTIL_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <fstream>
#include <string>
#include <vector>
#endif

#include "cpr/callback.h"
#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/secure_string.h"
#include "cpr/sse.h"

namespace cpr::util {

EXPORT_CPR Header parseHeader(const std::string& headers, std::string* status_line = nullptr, std::string* reason = nullptr);
EXPORT_CPR Cookies parseCookies(curl_slist* raw_cookies);
EXPORT_CPR size_t readUserFunction(char* ptr, size_t size, size_t nitems, const ReadCallback* read);
EXPORT_CPR size_t headerUserFunction(char* ptr, size_t size, size_t nmemb, const HeaderCallback* header);
EXPORT_CPR size_t writeFunction(char* ptr, size_t size, size_t nmemb, void* data);
EXPORT_CPR size_t writeFileFunction(char* ptr, size_t size, size_t nmemb, std::ofstream* file);
EXPORT_CPR size_t writeUserFunction(char* ptr, size_t size, size_t nmemb, const WriteCallback* write);
EXPORT_CPR size_t writeSSEFunction(char* ptr, size_t size, size_t nmemb, ServerSentEventCallback* sse);

EXPORT_CPR template <typename T = ProgressCallback>
int progressUserFunction(const T* progress, cpr_pf_arg_t dltotal, cpr_pf_arg_t dlnow, cpr_pf_arg_t ultotal, cpr_pf_arg_t ulnow) {
    const int cancel_retval{1};
#ifdef CURL_PROGRESSFUNC_CONTINUE // Not always defined. Ref: https://github.com/libcpr/cpr/issues/932
    static_assert(cancel_retval != CURL_PROGRESSFUNC_CONTINUE);
#endif // CURL_PROGRESSFUNC_CONTINUE
    return (*progress)(dltotal, dlnow, ultotal, ulnow) ? 0 : cancel_retval;
}
EXPORT_CPR int debugUserFunction(CURL* handle, curl_infotype type, char* data, size_t size, const DebugCallback* debug);
EXPORT_CPR std::vector<std::string> split(const std::string& to_split, char delimiter);
EXPORT_CPR util::SecureString urlEncode(std::string_view s);
EXPORT_CPR util::SecureString urlDecode(std::string_view s);

EXPORT_CPR bool isTrue(const std::string& s);

/**
 * Parses the given std::string into time_t (unix ms).
 * This parsing happens time_t size agnostic since time_t does not use the same underlying type on all systems/compilers.
 **/
EXPORT_CPR time_t sTimestampToT(const std::string& /*st*/);

} // namespace cpr::util

#endif
