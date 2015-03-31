#ifndef _CPR_UTIL_H_
#define _CPR_UTIL_H_

#include <curl/curl.h>

#include "cprtypes.h"


namespace cpr {
    namespace util {
        Header parseHeader(std::string headers);
        std::string parseResponse(std::string response);
        size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data);
        void freeHolder(CurlHolder* holder);
        CurlHolder* newHolder();
    }
};

#endif
