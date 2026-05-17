#ifndef CPR_CURLHOLDER_H
#define CPR_CURLHOLDER_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <array>
#include <mutex>
#endif

#include "cpr/secure_string.h"
#include <curl/curl.h>

namespace cpr {

EXPORT_CPR struct CurlHolder {
  private:
    /**
     * Mutex for curl_easy_init().
     * curl_easy_init() is not thread save.
     * References:
     * https://curl.haxx.se/libcurl/c/curl_easy_init.html
     * https://curl.haxx.se/libcurl/c/threadsafe.html
     **/

    // Avoids initalization order problems in a static build
    static std::mutex& curl_easy_init_mutex_() {
        static std::mutex curl_easy_init_mutex_;
        return curl_easy_init_mutex_;
    }

  public:
    CURL* handle{nullptr};
    struct curl_slist* chunk{nullptr};
    struct curl_slist* resolveCurlList{nullptr};
    curl_mime* multipart{nullptr};
    std::array<char, CURL_ERROR_SIZE> error{};

    CurlHolder();
    CurlHolder(const CurlHolder& other) = delete;
    CurlHolder(CurlHolder&& old) noexcept;
    ~CurlHolder();

    CurlHolder& operator=(const CurlHolder& other) = delete;
    CurlHolder& operator=(CurlHolder&& old) noexcept;

    /**
     * Uses curl_easy_escape(...) for escaping the given string.
     **/
    [[nodiscard]] util::SecureString urlEncode(std::string_view s) const;

    /**
     * Uses curl_easy_unescape(...) for unescaping the given string.
     **/
    [[nodiscard]] util::SecureString urlDecode(std::string_view s) const;
};
} // namespace cpr

#endif
