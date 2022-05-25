#if defined OPENSSL
#include "cpr/ssl_ctx.h"
#include <openssl/err.h>
#include <openssl/safestack.h>
#include <openssl/ssl.h>

namespace cpr {

// Implementation from: https://curl.se/libcurl/c/CURLOPT_SSL_CTX_DATA.html
CURLcode sslctx_function(CURL* /*curl*/, void* sslctx, void* parm) {
    X509_STORE* store = nullptr;
    X509* cert = nullptr;
    BIO* bio = nullptr;
    char* mypem = static_cast<char*>(parm);
    /* get a BIO */
    bio = BIO_new_mem_buf(mypem, -1);
    /* use it to read the PEM formatted certificate from memory into an
     * X509 structure that SSL can use
     */
    PEM_read_bio_X509(bio, &cert, 0, nullptr);
    if (cert == nullptr) {
        printf("PEM_read_bio_X509 failed...\n");
    }

    /* get a pointer to the X509 certificate store (which may be empty) */
    store = SSL_CTX_get_cert_store(static_cast<SSL_CTX*>(sslctx));

    /* add our certificate to this store */
    if (X509_STORE_add_cert(store, cert) == 0) {
        printf("error adding certificate\n");
    }

    /* decrease reference counts */
    X509_free(cert);
    BIO_free(bio);

    /* all set to go */
    return CURLE_OK;
}

} // namespace cpr
#endif