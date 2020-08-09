#ifndef CPR_UTIL_H
#define CPR_UTIL_H

#include <fstream>
#include <string>
#include <vector>

#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/curlholder.h"

namespace cpr {
namespace util {

Header parseHeader(const std::string& headers, std::string* status_line = nullptr,
                   std::string* reason = nullptr);
Cookies parseCookies(curl_slist* raw_cookies);
size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data);
size_t downloadFunction(void* ptr, size_t size, size_t nmemb, std::ofstream* file);
std::vector<std::string> split(const std::string& to_split, char delimiter);
std::string urlEncode(const std::string& s);

} // namespace util
} // namespace cpr

#endif
