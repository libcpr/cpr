#include "cpr/error.h"
#include <cstdint>
#include <curl/curl.h>
#include <curl/curlver.h>

namespace cpr {
ErrorCode Error::getErrorCodeForCurlError(std::int32_t curl_code) {
        switch (curl_code)
        {
        case CURLE_OK:
                return ErrorCode::OK;
        case CURLE_UNSUPPORTED_PROTOCOL:
                return ErrorCode::UNSUPPORTED_PROTOCOL;
        case CURLE_FAILED_INIT:
                return ErrorCode::FAILED_INIT;
        case CURLE_URL_MALFORMAT:
                return ErrorCode::URL_MALFORMAT;
        case CURLE_NOT_BUILT_IN:
                return ErrorCode::NOT_BUILT_IN;
        case CURLE_COULDNT_RESOLVE_PROXY:
                return ErrorCode::COULDNT_RESOLVE_PROXY;
        case CURLE_COULDNT_RESOLVE_HOST:
                return ErrorCode::COULDNT_RESOLVE_HOST;
        case CURLE_COULDNT_CONNECT:
                return ErrorCode::COULDNT_CONNECT;
        
// Name changed in curl >= 7.51.0.
#if LIBCURL_VERSION_NUM >= 0x073300
        case CURLE_WEIRD_SERVER_REPLY:
                return ErrorCode::WEIRD_SERVER_REPLY;
#else
        case CURLE_FTP_WEIRD_SERVER_REPLY:
                return ErrorCode::WEIRD_SERVER_REPLY;
#endif
        case CURLE_REMOTE_ACCESS_DENIED:
                return ErrorCode::REMOTE_ACCESS_DENIED;
        case CURLE_HTTP2:
                return ErrorCode::HTTP2;
        case CURLE_QUOTE_ERROR:
                return ErrorCode::QUOTE_ERROR;
        case CURLE_HTTP_RETURNED_ERROR:
                return ErrorCode::HTTP_RETURNED_ERROR;
        case CURLE_WRITE_ERROR:
                return ErrorCode::WRITE_ERROR;
        case CURLE_UPLOAD_FAILED:
                return ErrorCode::UPLOAD_FAILED;
        case CURLE_READ_ERROR:
                return ErrorCode::READ_ERROR;
        case CURLE_OUT_OF_MEMORY:
                return ErrorCode::OUT_OF_MEMORY;
        case CURLE_OPERATION_TIMEDOUT:
                return ErrorCode::OPERATION_TIMEDOUT;
        case CURLE_RANGE_ERROR:
                return ErrorCode::RANGE_ERROR;
        case CURLE_HTTP_POST_ERROR:
                return ErrorCode::HTTP_POST_ERROR;
        case CURLE_SSL_CONNECT_ERROR:
                return ErrorCode::SSL_CONNECT_ERROR;
        case CURLE_BAD_DOWNLOAD_RESUME:
                return ErrorCode::BAD_DOWNLOAD_RESUME;
        case CURLE_FILE_COULDNT_READ_FILE:
                return ErrorCode::FILE_COULDNT_READ_FILE;
        case CURLE_FUNCTION_NOT_FOUND:
                return ErrorCode::FUNCTION_NOT_FOUND;
        case CURLE_ABORTED_BY_CALLBACK:
                return ErrorCode::ABORTED_BY_CALLBACK;
        case CURLE_BAD_FUNCTION_ARGUMENT:
                return ErrorCode::BAD_FUNCTION_ARGUMENT;
        case CURLE_INTERFACE_FAILED:
                return ErrorCode::INTERFACE_FAILED;
        case CURLE_TOO_MANY_REDIRECTS:
                return ErrorCode::TOO_MANY_REDIRECTS;
        case CURLE_UNKNOWN_OPTION:
                return ErrorCode::UNKNOWN_OPTION;
// Added in curl 7.78.0.
#if LIBCURL_VERSION_NUM >= 0x074E00
        case CURLE_SETOPT_OPTION_SYNTAX:
                return ErrorCode::SETOPT_OPTION_SYNTAX;
#endif
        case CURLE_GOT_NOTHING:
                return ErrorCode::GOT_NOTHING;
        case CURLE_SSL_ENGINE_NOTFOUND:
                return ErrorCode::SSL_ENGINE_NOTFOUND;
        case CURLE_SSL_ENGINE_SETFAILED:
                return ErrorCode::SSL_ENGINE_SETFAILED;
        case CURLE_SEND_ERROR:
                return ErrorCode::SEND_ERROR;
        case CURLE_RECV_ERROR:
                return ErrorCode::RECV_ERROR;
        case CURLE_SSL_CERTPROBLEM:
                return ErrorCode::SSL_CERTPROBLEM;
        case CURLE_SSL_CIPHER:
                return ErrorCode::SSL_CIPHER;
        case CURLE_PEER_FAILED_VERIFICATION:
                return ErrorCode::PEER_FAILED_VERIFICATION;
        case CURLE_BAD_CONTENT_ENCODING:
                return ErrorCode::BAD_CONTENT_ENCODING;
        case CURLE_FILESIZE_EXCEEDED:
                return ErrorCode::FILESIZE_EXCEEDED;
        case CURLE_USE_SSL_FAILED:
                return ErrorCode::USE_SSL_FAILED;
        case CURLE_SEND_FAIL_REWIND:
                return ErrorCode::SEND_FAIL_REWIND;
        case CURLE_SSL_ENGINE_INITFAILED:
                return ErrorCode::SSL_ENGINE_INITFAILED;

// Added in curl 7.13.1.
#if LIBCURL_VERSION_NUM >= 0x070D01
        case CURLE_LOGIN_DENIED:
                return ErrorCode::LOGIN_DENIED;
#endif

// Added in curl 7.16.0.
#if LIBCURL_VERSION_NUM >= 0x071000
        case CURLE_SSL_CACERT_BADFILE:
                return ErrorCode::SSL_CACERT_BADFILE;
#endif

// Added in curl 7.16.1.
#if LIBCURL_VERSION_NUM >= 0x071001
        case CURLE_SSL_SHUTDOWN_FAILED:
                return ErrorCode::SSL_SHUTDOWN_FAILED;
#endif

// Added in curl 7.18.2.
#if LIBCURL_VERSION_NUM >= 0x071202
        case CURLE_AGAIN:
                return ErrorCode::AGAIN;
#endif

// Added in curl 7.19.0.
#if LIBCURL_VERSION_NUM >= 0x071300
        case CURLE_SSL_CRL_BADFILE:
                return ErrorCode::SSL_CRL_BADFILE;
        case CURLE_SSL_ISSUER_ERROR:
                return ErrorCode::SSL_ISSUER_ERROR;
#endif

// Added in curl 7.21.0.
#if LIBCURL_VERSION_NUM >= 0x071500
        case CURLE_CHUNK_FAILED:
                return ErrorCode::CHUNK_FAILED;
#endif

// Added in curl 7.30.0.
#if LIBCURL_VERSION_NUM >= 0x071E00
        case CURLE_NO_CONNECTION_AVAILABLE:
                return ErrorCode::NO_CONNECTION_AVAILABLE;
#endif

// Added in curl 7.39.0.
#if LIBCURL_VERSION_NUM >= 0x072700
        case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
                return ErrorCode::SSL_PINNEDPUBKEYNOTMATCH;
#endif

// Added in curl 7.41.0.
#if LIBCURL_VERSION_NUM >= 0x072900
        case CURLE_SSL_INVALIDCERTSTATUS:
                return ErrorCode::SSL_INVALIDCERTSTATUS;
#endif

// Added in curl 7.49.0.
#if LIBCURL_VERSION_NUM >= 0x073100
        case CURLE_HTTP2_STREAM:
                return ErrorCode::HTTP2_STREAM;
#endif
        case CURLE_PARTIAL_FILE:
                return ErrorCode::PARTIAL_FILE;

// Added in curl 7.59.0.
#if LIBCURL_VERSION_NUM >= 0x073B00
        case CURLE_RECURSIVE_API_CALL:
                return ErrorCode::RECURSIVE_API_CALL;
#endif

// Added in curl 7.66.0.
#if LIBCURL_VERSION_NUM >= 0x074200
        case CURLE_AUTH_ERROR:
                return ErrorCode::AUTH_ERROR;
#endif

// Added in curl 7.68.0.
#if LIBCURL_VERSION_NUM >= 0x074400
        case CURLE_HTTP3:
                return ErrorCode::HTTP3;
#endif

// Added in curl 7.69.0.
#if LIBCURL_VERSION_NUM >= 0x074500
        case CURLE_QUIC_CONNECT_ERROR:
                return ErrorCode::QUIC_CONNECT_ERROR;
#endif

// Added in curl 7.73.0.
#if LIBCURL_VERSION_NUM >= 0x074900
        case CURLE_PROXY:
                return ErrorCode::PROXY;
#endif

// Added in curl 7.77.0.
#if LIBCURL_VERSION_NUM >= 0x074D00
        case CURLE_SSL_CLIENTCERT:
                return ErrorCode::SSL_CLIENTCERT;
#endif

// Added in curl 7.84.0.
#if LIBCURL_VERSION_NUM >= 0x075400
        case CURLE_UNRECOVERABLE_POLL:
                return ErrorCode::UNRECOVERABLE_POLL;
#endif

// Added in curl 7.6.0.
#if LIBCURL_VERSION_NUM >= 0x080600
        case CURLE_TOO_LARGE:
                return ErrorCode::TOO_LARGE;
#endif
        default:
                return ErrorCode::UNKNOWN_ERROR;
        }
}

} // namespace cpr