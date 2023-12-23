#ifndef CPR_CURL_HOLDER_H
#define CPR_CURL_HOLDER_H

#include <array>
#include <curl/curl.h>
#include <mutex>
#include <string>

namespace cpr {
struct CurlHolder {
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
    CurlHolder(const CurlHolder& other) = default;
    CurlHolder(CurlHolder&& old) noexcept = default;
    ~CurlHolder();

    CurlHolder& operator=(CurlHolder&& old) noexcept = default;
    CurlHolder& operator=(const CurlHolder& other) = default;

    /**
     * Uses curl_easy_escape(...) for escaping the given string.
     **/
    [[nodiscard]] std::string urlEncode(const std::string& s) const;

    /**
     * Uses curl_easy_unescape(...) for unescaping the given string.
     **/
    [[nodiscard]] std::string urlDecode(const std::string& s) const;
};
} // namespace cpr

#endif
