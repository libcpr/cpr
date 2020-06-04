#ifndef CPR_CURL_HOLDER_H
#define CPR_CURL_HOLDER_H

#include <string>

#include <curl/curl.h>

namespace cpr {

struct CurlHolder {
    CURL* handle{nullptr};
    struct curl_slist* chunk{nullptr};
    struct curl_httppost* formpost{nullptr};
    char error[CURL_ERROR_SIZE];

    CurlHolder();
    ~CurlHolder();

    /**
     * Uses curl_easy_escape(...) for escaping the given string.
     **/
    std::string urlEncode(const std::string& s) const;
};

} // namespace cpr

#endif
