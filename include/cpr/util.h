#ifndef CPR_UTIL_H
#define CPR_UTIL_H

#include <fstream>
#include <string>
#include <vector>

#include "cpr/callback.h"
#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/curlholder.h"

namespace cpr {
namespace util {

Header parseHeader(const std::string& headers, std::string* status_line = nullptr,
                   std::string* reason = nullptr);
Cookies parseCookies(curl_slist* raw_cookies);
size_t readUserFunction(char* ptr, size_t size, size_t nitems, const ReadCallback* read);
size_t headerUserFunction(char* ptr, size_t size, size_t nmemb, const HeaderCallback* header);
size_t writeFunction(char* ptr, size_t size, size_t nmemb, std::string* data);
size_t writeFileFunction(char* ptr, size_t size, size_t nmemb, std::ofstream* file);
size_t writeUserFunction(char* ptr, size_t size, size_t nmemb, const WriteCallback* write);
#if LIBCURL_VERSION_NUM < 0x072000
int progressUserFunction(const ProgressCallback* progress, double dltotal, double dlnow,
                         double ultotal, double ulnow);
#else
int progressUserFunction(const ProgressCallback* progress, curl_off_t dltotal, curl_off_t dlnow,
                         curl_off_t ultotal, curl_off_t ulnow);
#endif
int debugUserFunction(CURL* handle, curl_infotype type, char* data, size_t size,
                      const DebugCallback* debug);
std::vector<std::string> split(const std::string& to_split, char delimiter);
std::string urlEncode(const std::string& s);
std::string urlDecode(const std::string& s);

} // namespace util
} // namespace cpr

#endif
