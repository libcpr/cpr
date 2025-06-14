
#include "cpr/ssl_ctx.h"
#include "cpr/ssl_options.h"
#include <cstddef>
#include <curl/curl.h>
#include <iostream>
#include <memory>

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION

#ifdef OPENSSL_BACKEND_USED

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

// openssl/types.h was added in later version of openssl (starting from 3.0.0) and is therefore not always available.
// This is for example the case on Ubuntu 20.04 or Centos 7.
// We try to include it if available to satisfy clang-tidy.
// Ref: https://github.com/openssl/openssl/commit/50cd4768c6b89c757645f28519236bb989216f8d
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/types.h>
#else
#include <openssl/ossl_typ.h>
#endif

// openssl/pemerr.h was added in 1.1.1a, but not in BoringSSL
// Ref https://github.com/libcpr/cpr/issues/333#issuecomment-2425104338
#if OPENSSL_VERSION_NUMBER >= 0x1010101fL && !defined(OPENSSL_IS_BORINGSSL)
#include <openssl/pemerr.h>
#endif

namespace cpr {

/**
 * The ssl_ctx parameter is actually a pointer to the SSL library's SSL_CTX for OpenSSL.
 * If an error is returned from the callback no attempt to establish a connection is made and
 * the perform operation will return the callback's error code.
 *
 * Sources: https://curl.se/libcurl/c/CURLOPT_SSL_CTX_FUNCTION.html
 *          https://curl.se/libcurl/c/CURLOPT_SSL_CTX_DATA.html
 */

template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};

template <typename T, auto fn>
using custom_unique_ptr = std::unique_ptr<T, deleter_from_fn<fn>>;
using x509_ptr = custom_unique_ptr<X509, X509_free>;
using bio_ptr = custom_unique_ptr<BIO, BIO_free>;

CURLcode sslctx_function_load_ca_cert_from_buffer(CURL* /*curl*/, void* sslctx, void* raw_cert_buf) {
    // Check arguments
    if (raw_cert_buf == nullptr || sslctx == nullptr) {
        std::cerr << "Invalid callback arguments!\n";
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // Create a memory BIO using the data of cert_buf
    // Note: It is assumed, that cert_buf is nul terminated and its length is determined by strlen
    char* cert_buf = static_cast<char*>(raw_cert_buf);
    BIO* bio = BIO_new_mem_buf(cert_buf, -1);

    // Get a pointer to the current certificate verification storage
    X509_STORE* store = SSL_CTX_get_cert_store(static_cast<SSL_CTX*>(sslctx));
    if (store == nullptr) {
        std::cerr << "SSL_CTX_get_cert_store failed!\n";
        ERR_print_errors_fp(stderr);
        BIO_free(bio);
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // Load the PEM formatted certicifate into an X509 structure which OpenSSL can use
    // PEM_read_bio_X509 can read multiple certificates from the same buffer in a loop.
    // The buffer should be in PEM format, which is a base64 encoded format
    // with header and footer lines like
    //
    //    CA 1
    //    ============
    //    -----BEGIN CERTIFICATE-----
    //    ... base64 data ...
    //    -----END CERTIFICATE-----
    //
    //    CA 2
    //    ============
    //    -----BEGIN CERTIFICATE-----
    //    ... base64 data ...
    //    -----END CERTIFICATE-----
    //
    size_t certs_loaded = 0;
    X509* cert = nullptr;
    while ((cert = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr)) != nullptr) {
        const int status = X509_STORE_add_cert(store, cert);
        // Fail if any loaded cert is invalid
        if (status == 0) {
            std::cerr << "[CPR] while adding certificate to store\n";
            ERR_print_errors_fp(stderr);
            BIO_free(bio);
            return CURLE_ABORTED_BY_CALLBACK;
        }
        certs_loaded++;
        // Free cert so we can load another one
        X509_free(cert);
        cert = nullptr;
    }

    // NOLINTNEXTLINE(google-runtime-int) Ignored here since it is an API return value
    const unsigned long err = ERR_peek_last_error();
    if (certs_loaded == 0 && err != 0) {
        // Check if the error is just EOF or an actual parsing error
        if (ERR_GET_LIB(err) == ERR_LIB_PEM && ERR_GET_REASON(err) == PEM_R_NO_START_LINE) {
            // This is expected if the buffer was empty or contains no valid
            // PEM certs
            std::cerr << "No PEM certificates found or end of stream\n";
        } else {
            std::cerr << "PEM_read_bio_X509 failed after loading " << certs_loaded << " certificates\n";
            ERR_print_errors_fp(stderr);
            BIO_free(bio);
            return CURLE_ABORTED_BY_CALLBACK;
        }
    }

    // Free the entire bio chain
    BIO_free(bio);

    // The CA certificate was loaded successfully into the verification storage
    return CURLE_OK;
}

} // namespace cpr

#endif // OPENSSL_BACKEND_USED

#endif // SUPPORT_CURLOPT_SSL_CTX_FUNCTION
