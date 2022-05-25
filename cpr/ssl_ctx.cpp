#include "cpr/ssl_ctx.h"
#include <openssl/err.h>
#include <openssl/safestack.h>
#include <openssl/ssl.h>

namespace cpr {

// Implementation from: https://curl.se/libcurl/c/cacertinmem.html
CURLcode sslctx_function(CURL* curl, void* sslctx, void* certBuffer) {
    CURLcode rv = CURLE_ABORTED_BY_CALLBACK;

    static const char* mypem = (char*) (certBuffer);

    BIO* cbio = BIO_new_mem_buf(mypem, sizeof(mypem));
    X509_STORE* cts = SSL_CTX_get_cert_store((SSL_CTX*) sslctx);
    int i;
    STACK_OF(X509_INFO) * inf;
    (void) curl;

    if (!cts || !cbio) {
        return rv;
    }

    inf = PEM_X509_INFO_read_bio(cbio, NULL, NULL, NULL);

    if (!inf) {
        BIO_free(cbio);
        return rv;
    }

    for (i = 0; i < sk_X509_INFO_num(inf); i++) {
        X509_INFO* itmp = sk_X509_INFO_value(inf, i);
        if (itmp->x509) {
            X509_STORE_add_cert(cts, itmp->x509);
        }
        if (itmp->crl) {
            X509_STORE_add_crl(cts, itmp->crl);
        }
    }

    sk_X509_INFO_pop_free(inf, X509_INFO_free);
    BIO_free(cbio);

    rv = CURLE_OK;
    return rv;
}

} // namespace cpr