#ifndef CPR_SSL_CTX_H
#define CPR_SSL_CTX_H

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
#include <curl/curl.h>

namespace cpr {

CURLcode sslctx_function(CURL* curl, void* sslctx, void* certBuffer);

} // Namespace cpr
#endif

#endif