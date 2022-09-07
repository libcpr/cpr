
#include "cpr/ssl_ctx.h"

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION

#ifdef OPENSSL_BACKEND_USED

#include <openssl/err.h>
#include <openssl/safestack.h>
#include <openssl/ssl.h>

namespace cpr {

/**
 * The ssl_ctx parameter is actually a pointer to the SSL library's SSL_CTX for OpenSSL.
 * If an error is returned from the callback no attempt to establish a connection is made and
 * the perform operation will return the callback's error code.
 *
 * Sources: https://curl.se/libcurl/c/CURLOPT_SSL_CTX_FUNCTION.html
 *          https://curl.se/libcurl/c/CURLOPT_SSL_CTX_DATA.html
 */
CURLcode sslctx_function_load_ca_cert_from_buffer(CURL* /*curl*/, void* sslctx, void* raw_cert_buf) {
    // Check arguments
    if (raw_cert_buf == nullptr || sslctx == nullptr) {
        printf("Invalid callback arguments\n");
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
        printf("PEM_read_bio_X509 failed\n");
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // Get a pointer to the current certificate verification storage
    store = SSL_CTX_get_cert_store(static_cast<SSL_CTX*>(sslctx));

    // Add the loaded certificate to the verification storage
    int status = X509_STORE_add_cert(store, cert);
    if (status == 0) {
        printf("Error adding certificate\n");
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // Decrement the reference count of the X509 structure cert and frees it up
    X509_free(cert);

    // Free the entire bio chain
    BIO_free(bio);

    // The CA certificate was loaded successfully into the verification storage
    return CURLE_OK;
}

} // namespace cpr

#endif // OPENSSL_BACKEND_USED

#endif // SUPPORT_CURLOPT_SSL_CTX_FUNCTION