#ifndef CPR_UTIL_H
#define CPR_UTIL_H

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
