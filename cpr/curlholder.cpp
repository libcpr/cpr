#include "cpr/curlholder.h"
#include "cpr/secure_string.h"
#include <cassert>
#include <curl/curl.h>
#include <curl/easy.h>
#include <string_view>

namespace cpr {
CurlHolder::CurlHolder() {
    /**
     * Allow multithreaded access to CPR by locking curl_easy_init().
     * curl_easy_init() is not thread safe.
     * References:
     * https://curl.haxx.se/libcurl/c/curl_easy_init.html
     * https://curl.haxx.se/libcurl/c/threadsafe.html
     **/
    curl_easy_init_mutex_().lock();
    // NOLINTNEXTLINE (cppcoreguidelines-prefer-member-initializer) since we need it to happen inside the lock
    handle = curl_easy_init();
    curl_easy_init_mutex_().unlock();

    assert(handle);
} // namespace cpr

CurlHolder::~CurlHolder() {
    curl_slist_free_all(chunk);
    curl_slist_free_all(resolveCurlList);
    curl_mime_free(multipart);
    curl_easy_cleanup(handle);
}

util::SecureString CurlHolder::urlEncode(std::string_view s) const {
    assert(handle);
    char* output = curl_easy_escape(handle, s.data(), static_cast<int>(s.length()));
    if (output) {
        util::SecureString result = output;
        curl_free(output);
        return result;
    }
    return "";
}

util::SecureString CurlHolder::urlDecode(std::string_view s) const {
    assert(handle);
    char* output = curl_easy_unescape(handle, s.data(), static_cast<int>(s.length()), nullptr);
    if (output) {
        util::SecureString result = output;
        curl_free(output);
        return result;
    }
    return "";
}
} // namespace cpr
