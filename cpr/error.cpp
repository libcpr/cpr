#include "error.h"
#include <curl/curl.h>

namespace cpr {

ErrorCode getErrorCodeForCurlError(int curlCode) {
    switch(curlCode) {
    case CURLE_OK: return ErrorCode::OK;

    case CURLE_UNSUPPORTED_PROTOCOL: return ErrorCode::UNSUPPORTED_PROTOCOL;
    case CURLE_URL_MALFORMAT: return ErrorCode::INVALID_URL_FORMAT;
    case CURLE_COULDNT_RESOLVE_PROXY: return ErrorCode::PROXY_RESOLUTION_FAILURE;
    case CURLE_COULDNT_RESOLVE_HOST: return ErrorCode::HOST_RESOLUTION_FAILURE;
    case CURLE_COULDNT_CONNECT: return ErrorCode::CONNECTION_FAILURE;
    case CURLE_HTTP2: return ErrorCode::GENERIC_HTTP2_FAILURE;
    case CURLE_SSL_CONNECT_ERROR: return ErrorCode::SSL_CONNECT_ERROR;
    case CURLE_PEER_FAILED_VERIFICATION: return ErrorCode::SSL_REMOTE_CERTIFICATE_ERROR;
    case CURLE_GOT_NOTHING: return ErrorCode::EMPTY_RESPONSE;
    case CURLE_SSL_ENGINE_NOTFOUND: return ErrorCode::GENERIC_SSL_ERROR;
    case CURLE_SSL_ENGINE_SETFAILED: return ErrorCode::GENERIC_SSL_ERROR;
    case CURLE_SSL_CERTPROBLEM: return ErrorCode::SSL_LOCAL_CERTIFICATE_ERROR;
    case CURLE_SSL_CIPHER: return ErrorCode::GENERIC_SSL_ERROR;
    case CURLE_SSL_CACERT: return ErrorCode::SSL_CACERT_ERROR;
    case CURLE_USE_SSL_FAILED: return ErrorCode::GENERIC_SSL_ERROR;
    case CURLE_SSL_ENGINE_INITFAILED: return ErrorCode::GENERIC_SSL_ERROR;
    case CURLE_SSL_CACERT_BADFILE: return ErrorCode::SSL_CACERT_ERROR;
    case CURLE_SSL_SHUTDOWN_FAILED: return ErrorCode::GENERIC_SSL_ERROR;
    case CURLE_SSL_CRL_BADFILE: return ErrorCode::SSL_CACERT_ERROR;
    case CURLE_SSL_ISSUER_ERROR: return ErrorCode::SSL_CACERT_ERROR;

    default: return ErrorCode::INTERNAL_ERROR;
    }

}

}