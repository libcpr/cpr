#ifndef CPR_SSL_CTX_H
#define CPR_SSL_CTX_H

#if defined OPENSSL
#include <curl/curl.h>

namespace cpr {

CURLcode sslctx_function(CURL* curl, void* sslctx, void* parm);

} // Namespace cpr
#endif

#endif