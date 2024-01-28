#include <functional>
#include <iostream>

#include "cpr/callback.h"
#include "cpr/cprtypes.h"
#include <curl/curl.h>

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

// openssl/types.h was added in later version of openssl and is therefore not always available.
// This is for example the case on Ubuntu 20.04.
// We try to include it if available to satisfy clang-tidy.
// Ref: https://github.com/openssl/openssl/commit/50cd4768c6b89c757645f28519236bb989216f8d
#if __has_include(<openssl/types.h>)
#include <openssl/types.h>
#else
#include <openssl/ossl_typ.h>
#endif
#endif // SUPPORT_CURLOPT_SSL_CTX_FUNCTION

namespace cpr {

void CancellationCallback::SetProgressCallback(ProgressCallback& u_cb) {
    user_cb.emplace(std::reference_wrapper{u_cb});
}
bool CancellationCallback::operator()(cpr_pf_arg_t dltotal, cpr_pf_arg_t dlnow, cpr_pf_arg_t ultotal, cpr_pf_arg_t ulnow) const {
    const bool cont_operation{!cancellation_state->load()};
    return user_cb ? (cont_operation && (*user_cb)(dltotal, dlnow, ultotal, ulnow)) : cont_operation;
}

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
namespace ssl {
/**
 * The ssl_ctx parameter is actually a pointer to the SSL library's SSL_CTX for OpenSSL.
 * If an error is returned from the callback no attempt to establish a connection is made and
 * the perform operation will return the callback's error code.
 *
 * Sources: https://curl.se/libcurl/c/CURLOPT_SSL_CTX_FUNCTION.html
 *          https://curl.se/libcurl/c/CURLOPT_SSL_CTX_DATA.html
 */
CURLcode tryLoadCaCertFromBuffer(CURL* /*curl*/, void* sslctx, void* raw_cert_buf) {
    // Check arguments
    if (raw_cert_buf == nullptr || sslctx == nullptr) {
        std::cerr << "CPR SSL context invalid callback arguments!\n";
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // Setup pointer
    X509_STORE* store = nullptr;
    X509* cert = nullptr;
    BIO* bio = nullptr;
    char* cert_buf = static_cast<char*>(raw_cert_buf);

    // Create a memory BIO using the data of cert_buf.
    // Note: It is assumed, that cert_buf is nul terminated and its length is determined by strlen.
    bio = BIO_new_mem_buf(cert_buf, -1);

    // Load the PEM formatted certicifate into an X509 structure which OpenSSL can use.
    PEM_read_bio_X509(bio, &cert, nullptr, nullptr);
    if (cert == nullptr) {
        std::cerr << "CPR SSL context PEM_read_bio_X509 failed!\n";
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // Get a pointer to the current certificate verification storage
    store = SSL_CTX_get_cert_store(static_cast<SSL_CTX*>(sslctx));

    // Add the loaded certificate to the verification storage
    const int status = X509_STORE_add_cert(store, cert);
    if (status == 0) {
        std::cerr << "CPR SSL context error adding certificate!\n";
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // Decrement the reference count of the X509 structure cert and frees it up
    X509_free(cert);

    // Free the entire bio chain
    BIO_free(bio);

    // The CA certificate was loaded successfully into the verification storage
    return CURLE_OK;
}
} // namespace ssl
#endif // SUPPORT_CURLOPT_SSL_CTX_FUNCTION
} // namespace cpr
