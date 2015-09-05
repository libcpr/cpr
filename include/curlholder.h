#ifndef CPR_CURL_HOLDER_H
#define CPR_CURL_HOLDER_H

#include <memory>

#include <curl/curl.h>

namespace cpr {

typedef struct {
    CURL* handle;
    struct curl_slist* chunk;
    char error[CURL_ERROR_SIZE];
} CurlHolder;


}

#endif
