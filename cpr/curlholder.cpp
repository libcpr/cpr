#include "cpr/curlholder.h"
#include <cassert>

namespace cpr {
CurlHolder::CurlHolder() : handle(curl_easy_init()) {
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
