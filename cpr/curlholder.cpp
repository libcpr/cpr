#include "cpr/curlholder.h"
#include <cassert>

namespace cpr {
std::mutex CurlHolder::curl_easy_init_mutex_{};

CurlHolder::CurlHolder() {
    /**
     * Allow multithreaded access to CPR by locking curl_easy_init().
     * curl_easy_init() is not thread safe.
     * References:
     * https://curl.haxx.se/libcurl/c/curl_easy_init.html
     * https://curl.haxx.se/libcurl/c/threadsafe.html
     **/
    curl_easy_init_mutex_.lock();
    handle = curl_easy_init();
    curl_easy_init_mutex_.unlock();

    assert(handle);
}

CurlHolder::~CurlHolder() {
    curl_easy_cleanup(handle);
    curl_slist_free_all(chunk);
    curl_formfree(formpost);
}

std::string CurlHolder::urlEncode(const std::string& s) const {
    assert(handle);
    char* output = curl_easy_escape(handle, s.c_str(), s.length());
    if (output) {
        std::string result = output;
        curl_free(output);
        return result;
    }
    return "";
}
} // namespace cpr
