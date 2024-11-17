#include "cpr/error.h"
#include <curl/curlver.h>
#include <unordered_map>
#include <cstdint>
#include <curl/curl.h>

namespace cpr {
static const std::unordered_map<std::int32_t, ErrorCode> curl_error_map = { // NOLINT - (needed because of static init)
        {CURLE_OK, ErrorCode::OK},
        {CURLE_UNSUPPORTED_PROTOCOL, ErrorCode::UNSUPPORTED_PROTOCOL},
        {CURLE_FAILED_INIT, ErrorCode::FAILED_INIT},
        {CURLE_URL_MALFORMAT, ErrorCode::URL_MALFORMAT},
        {CURLE_NOT_BUILT_IN, ErrorCode::NOT_BUILT_IN},
        {CURLE_COULDNT_RESOLVE_PROXY, ErrorCode::COULDNT_RESOLVE_PROXY},
        {CURLE_COULDNT_RESOLVE_HOST, ErrorCode::COULDNT_RESOLVE_HOST},
        {CURLE_COULDNT_CONNECT, ErrorCode::COULDNT_CONNECT},

// Name changed in curl >= 7.51.0.
#if LIBCURL_VERSION_NUM >= 0x073300
        {CURLE_WEIRD_SERVER_REPLY, ErrorCode::WEIRD_SERVER_REPLY},
#else
        {CURLE_FTP_WEIRD_SERVER_REPLY, ErrorCode::WEIRD_SERVER_REPLY},
#endif

        {CURLE_REMOTE_ACCESS_DENIED, ErrorCode::REMOTE_ACCESS_DENIED},
        {CURLE_HTTP2, ErrorCode::HTTP2},
        {CURLE_QUOTE_ERROR, ErrorCode::QUOTE_ERROR},
        {CURLE_HTTP_RETURNED_ERROR, ErrorCode::HTTP_RETURNED_ERROR},
        {CURLE_WRITE_ERROR, ErrorCode::WRITE_ERROR},
        {CURLE_UPLOAD_FAILED, ErrorCode::UPLOAD_FAILED},
        {CURLE_READ_ERROR, ErrorCode::READ_ERROR},
        {CURLE_OUT_OF_MEMORY, ErrorCode::OUT_OF_MEMORY},
        {CURLE_OPERATION_TIMEDOUT, ErrorCode::OPERATION_TIMEDOUT},
        {CURLE_RANGE_ERROR, ErrorCode::RANGE_ERROR},
        {CURLE_HTTP_POST_ERROR, ErrorCode::HTTP_POST_ERROR},
        {CURLE_SSL_CONNECT_ERROR, ErrorCode::SSL_CONNECT_ERROR},
        {CURLE_BAD_DOWNLOAD_RESUME, ErrorCode::BAD_DOWNLOAD_RESUME},
        {CURLE_FILE_COULDNT_READ_FILE, ErrorCode::FILE_COULDNT_READ_FILE},
        {CURLE_FUNCTION_NOT_FOUND, ErrorCode::FUNCTION_NOT_FOUND},
        {CURLE_ABORTED_BY_CALLBACK, ErrorCode::ABORTED_BY_CALLBACK},
        {CURLE_BAD_FUNCTION_ARGUMENT, ErrorCode::BAD_FUNCTION_ARGUMENT},
        {CURLE_INTERFACE_FAILED, ErrorCode::INTERFACE_FAILED},
        {CURLE_TOO_MANY_REDIRECTS, ErrorCode::TOO_MANY_REDIRECTS},
        {CURLE_UNKNOWN_OPTION, ErrorCode::UNKNOWN_OPTION},

// Added in curl 7.78.0.
#if LIBCURL_VERSION_NUM >= 0x074E00
        {CURLE_SETOPT_OPTION_SYNTAX, ErrorCode::SETOPT_OPTION_SYNTAX},
#endif

        {CURLE_GOT_NOTHING, ErrorCode::GOT_NOTHING},
        {CURLE_SSL_ENGINE_NOTFOUND, ErrorCode::SSL_ENGINE_NOTFOUND},
        {CURLE_SSL_ENGINE_SETFAILED, ErrorCode::SSL_ENGINE_SETFAILED},
        {CURLE_SEND_ERROR, ErrorCode::SEND_ERROR},
        {CURLE_RECV_ERROR, ErrorCode::RECV_ERROR},
        {CURLE_SSL_CERTPROBLEM, ErrorCode::SSL_CERTPROBLEM},
        {CURLE_SSL_CIPHER, ErrorCode::SSL_CIPHER},
        {CURLE_PEER_FAILED_VERIFICATION, ErrorCode::PEER_FAILED_VERIFICATION},
        {CURLE_BAD_CONTENT_ENCODING, ErrorCode::BAD_CONTENT_ENCODING},
        {CURLE_FILESIZE_EXCEEDED, ErrorCode::FILESIZE_EXCEEDED},
        {CURLE_USE_SSL_FAILED, ErrorCode::USE_SSL_FAILED},
        {CURLE_SEND_FAIL_REWIND, ErrorCode::SEND_FAIL_REWIND},
        {CURLE_SSL_ENGINE_INITFAILED, ErrorCode::SSL_ENGINE_INITFAILED},

// Added in curl 7.13.1.
#if LIBCURL_VERSION_NUM >= 0x070D01
        {CURLE_LOGIN_DENIED, ErrorCode::LOGIN_DENIED},
#endif

// Added in curl 7.16.0.
#if LIBCURL_VERSION_NUM >= 0x071000
        {CURLE_SSL_CACERT_BADFILE, ErrorCode::SSL_CACERT_BADFILE},
#endif

// Added in curl 7.16.1.
#if LIBCURL_VERSION_NUM >= 0x071001
        {CURLE_SSL_SHUTDOWN_FAILED, ErrorCode::SSL_SHUTDOWN_FAILED},
#endif

// Added in curl 7.18.2.
#if LIBCURL_VERSION_NUM >= 0x071202
        {CURLE_AGAIN, ErrorCode::AGAIN},
#endif

// Added in curl 7.19.0.
#if LIBCURL_VERSION_NUM >= 0x071300
        {CURLE_SSL_CRL_BADFILE, ErrorCode::SSL_CRL_BADFILE},
        {CURLE_SSL_ISSUER_ERROR, ErrorCode::SSL_ISSUER_ERROR},
#endif

// Added in curl 7.21.0.
#if LIBCURL_VERSION_NUM >= 0x071500
        {CURLE_CHUNK_FAILED, ErrorCode::CHUNK_FAILED},
#endif

// Added in curl 7.30.0.
#if LIBCURL_VERSION_NUM >= 0x071E00
        {CURLE_NO_CONNECTION_AVAILABLE, ErrorCode::NO_CONNECTION_AVAILABLE},
#endif

// Added in curl 7.39.0.
#if LIBCURL_VERSION_NUM >= 0x072700
        {CURLE_SSL_PINNEDPUBKEYNOTMATCH, ErrorCode::SSL_PINNEDPUBKEYNOTMATCH},
#endif

// Added in curl 7.41.0.
#if LIBCURL_VERSION_NUM >= 0x072900
        {CURLE_SSL_INVALIDCERTSTATUS, ErrorCode::SSL_INVALIDCERTSTATUS},
#endif

// Added in curl 7.49.0.
#if LIBCURL_VERSION_NUM >= 0x073100
        {CURLE_HTTP2_STREAM, ErrorCode::HTTP2_STREAM},
#endif

        {CURLE_PARTIAL_FILE, ErrorCode::PARTIAL_FILE},

// Added in curl 7.59.0.
#if LIBCURL_VERSION_NUM >= 0x073B00
        {CURLE_RECURSIVE_API_CALL, ErrorCode::RECURSIVE_API_CALL},
#endif

// Added in curl 7.66.0.
#if LIBCURL_VERSION_NUM >= 0x074200
        {CURLE_AUTH_ERROR, ErrorCode::AUTH_ERROR},
#endif

// Added in curl 7.68.0.
#if LIBCURL_VERSION_NUM >= 0x074400
        {CURLE_HTTP3, ErrorCode::HTTP3},
#endif

// Added in curl 7.69.0.
#if LIBCURL_VERSION_NUM >= 0x074500
        {CURLE_QUIC_CONNECT_ERROR, ErrorCode::QUIC_CONNECT_ERROR},
#endif

// Added in curl 7.73.0.
#if LIBCURL_VERSION_NUM >= 0x074900
        {CURLE_PROXY, ErrorCode::PROXY},
#endif

// Added in curl 7.77.0.
#if LIBCURL_VERSION_NUM >= 0x074D00
        {CURLE_SSL_CLIENTCERT, ErrorCode::SSL_CLIENTCERT},
#endif

// Added in curl 7.84.0.
#if LIBCURL_VERSION_NUM >= 0x075400
        {CURLE_UNRECOVERABLE_POLL, ErrorCode::UNRECOVERABLE_POLL},
#endif

// Added in curl 7.6.0.
#if LIBCURL_VERSION_NUM >= 0x080600
        {CURLE_TOO_LARGE, ErrorCode::TOO_LARGE},
#endif
};

ErrorCode Error::getErrorCodeForCurlError(std::int32_t curl_code) {
    auto it = curl_error_map.find(curl_code);
    if (it == curl_error_map.end()) {
        // Default return value when the CURL error code is not recognized
        return ErrorCode::UNKNOWN_ERROR;
    }
    return it->second;
}
} // namespace cpr