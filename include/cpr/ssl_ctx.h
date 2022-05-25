#ifndef CPR_SSL_CTX_H
#define CPR_SSL_CTX_H

#include <curl/curl.h>

namespace cpr {

CURLcode sslctx_function(CURL* curl, void* sslctx, void* certBuffer);

} // Namespace cpr

#endif