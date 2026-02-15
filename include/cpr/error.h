#ifndef CPR_ERROR_H
#define CPR_ERROR_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "cpr/cprtypes.h"
#include <utility>

namespace cpr {

/**
 * cpr error codes that match the ones found inside 'curl.h'.
 * These error codes only include relevant error codes meaning no support for e.g. FTP errors since cpr does only support HTTP.
 **/
enum class ErrorCode : uint16_t {
    /**
     * Everything is good and no error occurred.
     **/
    OK = 0,
    UNSUPPORTED_PROTOCOL = 1,
    FAILED_INIT = 2,
    URL_MALFORMAT = 3,
    NOT_BUILT_IN = 4,
    COULDNT_RESOLVE_PROXY = 5,
    COULDNT_RESOLVE_HOST = 6,
    COULDNT_CONNECT = 7,
    WEIRD_SERVER_REPLY = 8,
    REMOTE_ACCESS_DENIED = 9,
    HTTP2 = 10,
    PARTIAL_FILE = 11,
    QUOTE_ERROR = 12,
    HTTP_RETURNED_ERROR = 13,
    WRITE_ERROR = 14,
    UPLOAD_FAILED = 15,
    READ_ERROR = 16,
    OUT_OF_MEMORY = 17,
    OPERATION_TIMEDOUT = 18,
    RANGE_ERROR = 19,
    HTTP_POST_ERROR = 20,
    SSL_CONNECT_ERROR = 21,
    BAD_DOWNLOAD_RESUME = 22,
    FILE_COULDNT_READ_FILE = 23,
    FUNCTION_NOT_FOUND = 24,
    ABORTED_BY_CALLBACK = 25,
    BAD_FUNCTION_ARGUMENT = 26,
    INTERFACE_FAILED = 27,
    TOO_MANY_REDIRECTS = 28,
    UNKNOWN_OPTION = 29,
    SETOPT_OPTION_SYNTAX = 30,
    GOT_NOTHING = 31,
    SSL_ENGINE_NOTFOUND = 32,
    SSL_ENGINE_SETFAILED = 33,
    SEND_ERROR = 34,
    RECV_ERROR = 35,
    SSL_CERTPROBLEM = 36,
    SSL_CIPHER = 37,
    PEER_FAILED_VERIFICATION = 38,
    BAD_CONTENT_ENCODING = 39,
    FILESIZE_EXCEEDED = 40,
    USE_SSL_FAILED = 41,
    SEND_FAIL_REWIND = 42,
    SSL_ENGINE_INITFAILED = 43,
    LOGIN_DENIED = 44,
    SSL_CACERT_BADFILE = 45,
    SSL_SHUTDOWN_FAILED = 46,
    AGAIN = 47,
    SSL_CRL_BADFILE = 48,
    SSL_ISSUER_ERROR = 49,
    CHUNK_FAILED = 50,
    NO_CONNECTION_AVAILABLE = 51,
    SSL_PINNEDPUBKEYNOTMATCH = 52,
    SSL_INVALIDCERTSTATUS = 53,
    HTTP2_STREAM = 54,
    RECURSIVE_API_CALL = 55,
    AUTH_ERROR = 56,
    HTTP3 = 57,
    QUIC_CONNECT_ERROR = 58,
    PROXY = 59,
    SSL_CLIENTCERT = 60,
    UNRECOVERABLE_POLL = 61,
    TOO_LARGE = 62,
    /**
     * An unknown error inside curl occurred.
     * Please try to reproduce it and then report it to us.
     * It might be that there is a new curl error code we are not aware yet.
     * Reporting bugs: https://github.com/libcpr/cpr
     **/
    UNKNOWN_ERROR = 1000,
};

inline const std::unordered_map<ErrorCode, std::string>& get_error_code_to_string_mapping() {
    // Use a function-local static rather than inline global objects to avoid the 'double-destructor' problem in MSVC when using /MT flags.
    static const std::unordered_map<ErrorCode, std::string> mapping = {{ErrorCode::OK, "OK"},
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
    return mapping;
}

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

// NOLINTBEGIN(cert-dcl58-cpp) required for to_string
namespace std {
inline std::string to_string(const cpr::ErrorCode& code) {
    return cpr::get_error_code_to_string_mapping().at(code);
}
} // namespace std
// NOLINTEND(cert-dcl58-cpp) required for to_string


#endif
