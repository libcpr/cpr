#include <functional>
#ifdef OPENSSL_BACKEND_USED
#include <cstddef>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#endif // OPENSSL_BACKEND_USED

#include "cpr/callback.h"
#include "cpr/cprtypes.h"
#include <curl/curl.h>

#ifdef OPENSSL_BACKEND_USED
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/pemerr.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

// openssl/types.h was added in later version of openssl and is therefore not always available.
// This is for example the case on Ubuntu 20.04.
// We try to include it if available to satisfy clang-tidy.
// Ref: https://github.com/openssl/openssl/commit/50cd4768c6b89c757645f28519236bb989216f8d
// cppcheck-suppress preprocessorErrorDirective
#if __has_include(<openssl/types.h>)
#include <openssl/types.h>
#else
#include <openssl/ossl_typ.h>
#endif
#endif // OPENSSL_BACKEND_USED

namespace cpr {

void CancellationCallback::SetProgressCallback(ProgressCallback& u_cb) {
    user_cb.emplace(std::reference_wrapper{u_cb});
}
bool CancellationCallback::operator()(cpr_pf_arg_t dltotal, cpr_pf_arg_t dlnow, cpr_pf_arg_t ultotal, cpr_pf_arg_t ulnow) const {
    const bool cont_operation{!cancellation_state->load()};
    return user_cb ? (cont_operation && (*user_cb)(dltotal, dlnow, ultotal, ulnow)) : cont_operation;
}

#ifdef OPENSSL_BACKEND_USED
namespace ssl {
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

inline std::string get_openssl_print_errors() {
    std::ostringstream oss;
    ERR_print_errors_cb(
            [](char const* str, size_t len, void* data) -> int {
                auto& oss = *static_cast<std::ostringstream*>(data);
                oss << str;
                return static_cast<int>(len);
            },
            &oss);
    return oss.str();
}

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
        std::cerr << "Invalid callback arguments!\n";
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // Get a pointer to the current certificate verification storage
    auto* store = SSL_CTX_get_cert_store(static_cast<SSL_CTX*>(sslctx));

    // Create a memory BIO using the data of cert_buf.
    // Note: It is assumed, that cert_buf is nul terminated and its length is determined by strlen.
    const bio_ptr bio{BIO_new_mem_buf(static_cast<char*>(raw_cert_buf), -1)};

    bool at_least_got_one = false;
    for (;;) {
        // Load the PEM formatted certicifate into an X509 structure which OpenSSL can use.
        const x509_ptr x{PEM_read_bio_X509_AUX(bio.get(), nullptr, nullptr, nullptr)};
        if (x == nullptr) {
            if ((ERR_GET_REASON(ERR_peek_last_error()) == PEM_R_NO_START_LINE) && at_least_got_one) {
                ERR_clear_error();
                break;
            }
            std::cerr << "PEM_read_bio_X509_AUX failed: \n" << get_openssl_print_errors() << '\n';
            return CURLE_ABORTED_BY_CALLBACK;
        }

        // Add the loaded certificate to the verification storage
        if (X509_STORE_add_cert(store, x.get()) == 0) {
            std::cerr << "X509_STORE_add_cert failed: \n" << get_openssl_print_errors() << '\n';
            return CURLE_ABORTED_BY_CALLBACK;
        }
        at_least_got_one = true;
    }

    // The CA certificate was loaded successfully into the verification storage
    return CURLE_OK;
}
} // namespace ssl
#endif // OPENSSL_BACKEND_USED
} // namespace cpr
