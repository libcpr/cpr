#ifndef CPR_ERROR_H
#define CPR_ERROR_H

#include <unordered_map>
#include <cstdint>
#include <string>

#include "cpr/cprtypes.h"
#include <utility>

namespace cpr {

/**
 * cpr error codes that match the ones found inside 'curl.h'.
 * These error codes only include relevant error codes meaning no support for e.g. FTP errors since cpr does only support HTTP.
 **/
enum class ErrorCode {
    /**
     * Everything is good and no error occurred.
     **/
    OK = 0,
    UNSUPPORTED_PROTOCOL,
    FAILED_INIT,
    URL_MALFORMAT,
    NOT_BUILT_IN,
    COULDNT_RESOLVE_PROXY,
    COULDNT_RESOLVE_HOST,
    COULDNT_CONNECT,
    WEIRD_SERVER_REPLY,
    REMOTE_ACCESS_DENIED,
    HTTP2,
    PARTIAL_FILE,
    QUOTE_ERROR,
    HTTP_RETURNED_ERROR,
    WRITE_ERROR,
    UPLOAD_FAILED,
    READ_ERROR,
    OUT_OF_MEMORY,
    OPERATION_TIMEDOUT,
    RANGE_ERROR,
    HTTP_POST_ERROR,
    SSL_CONNECT_ERROR,
    BAD_DOWNLOAD_RESUME,
    FILE_COULDNT_READ_FILE,
    FUNCTION_NOT_FOUND,
    ABORTED_BY_CALLBACK,
    BAD_FUNCTION_ARGUMENT,
    INTERFACE_FAILED,
    TOO_MANY_REDIRECTS,
    UNKNOWN_OPTION,
    SETOPT_OPTION_SYNTAX,
    GOT_NOTHING,
    SSL_ENGINE_NOTFOUND,
    SSL_ENGINE_SETFAILED,
    SEND_ERROR,
    RECV_ERROR,
    SSL_CERTPROBLEM,
    SSL_CIPHER,
    PEER_FAILED_VERIFICATION,
    BAD_CONTENT_ENCODING,
    FILESIZE_EXCEEDED,
    USE_SSL_FAILED,
    SEND_FAIL_REWIND,
    SSL_ENGINE_INITFAILED,
    LOGIN_DENIED,
    SSL_CACERT_BADFILE,
    SSL_SHUTDOWN_FAILED,
    AGAIN,
    SSL_CRL_BADFILE,
    SSL_ISSUER_ERROR,
    CHUNK_FAILED,
    NO_CONNECTION_AVAILABLE,
    SSL_PINNEDPUBKEYNOTMATCH,
    SSL_INVALIDCERTSTATUS,
    HTTP2_STREAM,
    RECURSIVE_API_CALL,
    AUTH_ERROR,
    HTTP3,
    QUIC_CONNECT_ERROR,
    PROXY,
    SSL_CLIENTCERT,
    UNRECOVERABLE_POLL,
    TOO_LARGE,
    /**
     * An unknown error inside curl occurred.
     * Please try to reproduce it and then report it to us.
     * It might be that there is a new curl error code we are not aware yet.
     * Reporting bugs: https://github.com/libcpr/cpr
     **/
    UNKNOWN_ERROR = 1000,
};

inline const std::unordered_map<ErrorCode, std::string> error_code_to_string_mapping = {{ErrorCode::OK, "OK"},
                                                                                  {ErrorCode::UNSUPPORTED_PROTOCOL, "UNSUPPORTED_PROTOCOL"},
                                                                                  {ErrorCode::FAILED_INIT, "FAILED_INIT"},
                                                                                  {ErrorCode::URL_MALFORMAT, "URL_MALFORMAT"},
                                                                                  {ErrorCode::NOT_BUILT_IN, "NOT_BUILT_IN"},
                                                                                  {ErrorCode::COULDNT_RESOLVE_PROXY, "COULDNT_RESOLVE_PROXY"},
                                                                                  {ErrorCode::COULDNT_RESOLVE_HOST, "COULDNT_RESOLVE_HOST"},
                                                                                  {ErrorCode::COULDNT_CONNECT, "COULDNT_CONNECT"},
                                                                                  {ErrorCode::WEIRD_SERVER_REPLY, "WEIRD_SERVER_REPLY"},
                                                                                  {ErrorCode::REMOTE_ACCESS_DENIED, "REMOTE_ACCESS_DENIED"},
                                                                                  {ErrorCode::HTTP2, "HTTP2"},
                                                                                  {ErrorCode::PARTIAL_FILE, "PARTIAL_FILE"},
                                                                                  {ErrorCode::QUOTE_ERROR, "QUOTE_ERROR"},
                                                                                  {ErrorCode::HTTP_RETURNED_ERROR, "HTTP_RETURNED_ERROR"},
                                                                                  {ErrorCode::WRITE_ERROR, "WRITE_ERROR"},
                                                                                  {ErrorCode::UPLOAD_FAILED, "UPLOAD_FAILED"},
                                                                                  {ErrorCode::READ_ERROR, "READ_ERROR"},
                                                                                  {ErrorCode::OUT_OF_MEMORY, "OUT_OF_MEMORY"},
                                                                                  {ErrorCode::OPERATION_TIMEDOUT, "OPERATION_TIMEDOUT"},
                                                                                  {ErrorCode::RANGE_ERROR, "RANGE_ERROR"},
                                                                                  {ErrorCode::HTTP_POST_ERROR, "HTTP_POST_ERROR"},
                                                                                  {ErrorCode::SSL_CONNECT_ERROR, "SSL_CONNECT_ERROR"},
                                                                                  {ErrorCode::BAD_DOWNLOAD_RESUME, "BAD_DOWNLOAD_RESUME"},
                                                                                  {ErrorCode::FILE_COULDNT_READ_FILE, "FILE_COULDNT_READ_FILE"},
                                                                                  {ErrorCode::FUNCTION_NOT_FOUND, "FUNCTION_NOT_FOUND"},
                                                                                  {ErrorCode::ABORTED_BY_CALLBACK, "ABORTED_BY_CALLBACK"},
                                                                                  {ErrorCode::BAD_FUNCTION_ARGUMENT, "BAD_FUNCTION_ARGUMENT"},
                                                                                  {ErrorCode::INTERFACE_FAILED, "INTERFACE_FAILED"},
                                                                                  {ErrorCode::TOO_MANY_REDIRECTS, "TOO_MANY_REDIRECTS"},
                                                                                  {ErrorCode::UNKNOWN_OPTION, "UNKNOWN_OPTION"},
                                                                                  {ErrorCode::SETOPT_OPTION_SYNTAX, "SETOPT_OPTION_SYNTAX"},
                                                                                  {ErrorCode::GOT_NOTHING, "GOT_NOTHING"},
                                                                                  {ErrorCode::SSL_ENGINE_NOTFOUND, "SSL_ENGINE_NOTFOUND"},
                                                                                  {ErrorCode::SSL_ENGINE_SETFAILED, "SSL_ENGINE_SETFAILED"},
                                                                                  {ErrorCode::SEND_ERROR, "SEND_ERROR"},
                                                                                  {ErrorCode::RECV_ERROR, "RECV_ERROR"},
                                                                                  {ErrorCode::SSL_CERTPROBLEM, "SSL_CERTPROBLEM"},
                                                                                  {ErrorCode::SSL_CIPHER, "SSL_CIPHER"},
                                                                                  {ErrorCode::PEER_FAILED_VERIFICATION, "PEER_FAILED_VERIFICATION"},
                                                                                  {ErrorCode::BAD_CONTENT_ENCODING, "BAD_CONTENT_ENCODING"},
                                                                                  {ErrorCode::FILESIZE_EXCEEDED, "FILESIZE_EXCEEDED"},
                                                                                  {ErrorCode::USE_SSL_FAILED, "USE_SSL_FAILED"},
                                                                                  {ErrorCode::SEND_FAIL_REWIND, "SEND_FAIL_REWIND"},
                                                                                  {ErrorCode::SSL_ENGINE_INITFAILED, "SSL_ENGINE_INITFAILED"},
                                                                                  {ErrorCode::LOGIN_DENIED, "LOGIN_DENIED"},
                                                                                  {ErrorCode::SSL_CACERT_BADFILE, "SSL_CACERT_BADFILE"},
                                                                                  {ErrorCode::SSL_SHUTDOWN_FAILED, "SSL_SHUTDOWN_FAILED"},
                                                                                  {ErrorCode::AGAIN, "AGAIN"},
                                                                                  {ErrorCode::SSL_CRL_BADFILE, "SSL_CRL_BADFILE"},
                                                                                  {ErrorCode::SSL_ISSUER_ERROR, "SSL_ISSUER_ERROR"},
                                                                                  {ErrorCode::CHUNK_FAILED, "CHUNK_FAILED"},
                                                                                  {ErrorCode::NO_CONNECTION_AVAILABLE, "NO_CONNECTION_AVAILABLE"},
                                                                                  {ErrorCode::SSL_PINNEDPUBKEYNOTMATCH, "SSL_PINNEDPUBKEYNOTMATCH"},
                                                                                  {ErrorCode::SSL_INVALIDCERTSTATUS, "SSL_INVALIDCERTSTATUS"},
                                                                                  {ErrorCode::HTTP2_STREAM, "HTTP2_STREAM"},
                                                                                  {ErrorCode::RECURSIVE_API_CALL, "RECURSIVE_API_CALL"},
                                                                                  {ErrorCode::AUTH_ERROR, "AUTH_ERROR"},
                                                                                  {ErrorCode::HTTP3, "HTTP3"},
                                                                                  {ErrorCode::QUIC_CONNECT_ERROR, "QUIC_CONNECT_ERROR"},
                                                                                  {ErrorCode::PROXY, "PROXY"},
                                                                                  {ErrorCode::SSL_CLIENTCERT, "SSL_CLIENTCERT"},
                                                                                  {ErrorCode::UNRECOVERABLE_POLL, "UNRECOVERABLE_POLL"},
                                                                                  {ErrorCode::TOO_LARGE, "TOO_LARGE"},
                                                                                  {ErrorCode::UNKNOWN_ERROR, "UNKNOWN_ERROR"}};

class Error {
  public:
    ErrorCode code = ErrorCode::OK;
    std::string message;

    Error() = default;

    Error(const std::int32_t& curl_code, std::string&& p_error_message) : code{getErrorCodeForCurlError(curl_code)}, message(std::move(p_error_message)) {}

    explicit operator bool() const {
        return code != ErrorCode::OK;
    }

  private:
    static ErrorCode getErrorCodeForCurlError(std::int32_t curl_code);
};

} // namespace cpr

namespace std {
inline std::string to_string(const cpr::ErrorCode& code) {
    return cpr::error_code_to_string_mapping.at(code);
}
} // namespace std


#endif
