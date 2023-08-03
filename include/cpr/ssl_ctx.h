#ifndef CPR_SSL_CTX_H
#define CPR_SSL_CTX_H

#include "cpr/ssl_options.h"
#include <curl/curl.h>

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION

namespace cpr {

/**
 * This callback function loads a CA certificate from raw_cert_buf and gets called by libcurl
 * just before the initialization of an SSL connection.
 * The raw_cert_buf argument is set with the CURLOPT_SSL_CTX_DATA option and has to be a nul
 * terminated buffer.
 *
 * Sources: https://curl.se/libcurl/c/CURLOPT_SSL_CTX_FUNCTION.html
 *         https://curl.se/libcurl/c/CURLOPT_SSL_CTX_DATA.html
 */
CURLcode sslctx_function_load_ca_cert_from_buffer(CURL* curl, void* sslctx, void* raw_cert_buf);

} // Namespace cpr

#endif

#endif
