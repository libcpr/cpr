#include "cpr/curlmultiholder.h"
#include <cassert>
#include <curl/multi.h>

namespace cpr {

CurlMultiHolder::CurlMultiHolder() : handle{curl_multi_init()} {
    assert(handle);
}

CurlMultiHolder::~CurlMultiHolder() {
    curl_multi_cleanup(handle);
}

} // namespace cpr
