#ifndef CPR_UTIL_H
#define CPR_UTIL_H

#include "cprtypes.h"


namespace cpr {
    namespace util {
        Header parseHeader(const std::string& headers);
        std::string parseResponse(const std::string& response);
        size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data);
    }
};

#endif
