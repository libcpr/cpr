#include "cpr/curlholder.h"
#include <cassert>

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
    curl_formfree(formpost);
    curl_easy_cleanup(handle);
}

std::string CurlHolder::urlEncode(const std::string& s) const {
    assert(handle);
    char* output = curl_easy_escape(handle, s.c_str(), static_cast<int>(s.length()));
    if (output) {
        std::string result = output;
        curl_free(output);
        return result;
    }
    return "";
}

std::string CurlHolder::urlDecode(const std::string& s) const {
    assert(handle);
    char* output = curl_easy_unescape(handle, s.c_str(), static_cast<int>(s.length()), nullptr);
    if (output) {
        std::string result = output;
        curl_free(output);
        return result;
    }
    return "";
}
} // namespace cpr
