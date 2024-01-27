#ifndef CPR_SSL_CTX_H
#define CPR_SSL_CTX_H

#include "cpr/ssl_options.h"
#include <cassert>

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION

#include "curlholder.h"
#include <curl/curl.h>

namespace cpr {

/**
 * This callback function gets called by libcurl just before the initialization of an SSL connection
 * after having processed all other SSL related options to give a last chance to an application
 * to modify the behavior of the SSL initialization.
 *
 * If an error is returned from the callback no attempt to establish a connection is made
 * and the perform operation returns the callback's error code.
 * For no error return CURLE_OK from inside 'curl/curl.h'
 *
 * More/Source: https://curl.se/libcurl/c/CURLOPT_SSL_CTX_FUNCTION.html
 **/
class SslCtxCallback {
  public:
    std::function<CURLcode(const std::shared_ptr<CurlHolder>& curl_holder, void* ssl_ctx, intptr_t userdata)> callback{};
    intptr_t userdata{};
    std::shared_ptr<CurlHolder> curl_holder{nullptr};

    SslCtxCallback() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    SslCtxCallback(std::function<CURLcode(const std::shared_ptr<CurlHolder>& p_curl_holder, void* p_ssl_ctx, intptr_t p_userdata)>& p_callback, intptr_t p_userdata = 0) : callback(p_callback), userdata(p_userdata) {}

    CURLcode operator()(CURL* p_curl, void* p_ssl_ctx, void* p_clientp) const {
        // We use our own way of passing arguments curl and the client pointer to the function.
        assert(p_curl == curl_holder->handle);
        assert(!p_clientp);

        return callback(curl_holder, p_ssl_ctx, userdata);
    }

    void SetCurlHolder(const std::shared_ptr<CurlHolder>& p_curl_holder) {
        this->curl_holder = p_curl_holder;
    }
};

CURLcode tryLoadCaCertFromBuffer(CURL* curl, void* sslctx, void* raw_cert_buf);

} // Namespace cpr

#endif

#endif
