#ifndef CPR_ERROR_H
#define CPR_ERROR_H

#include <cstdint>
#include <string>

#include "cpr/cprtypes.h"
#include <utility>

namespace cpr {

enum class ErrorCode {
    OK = 0,
    CONNECTION_FAILURE,
    EMPTY_RESPONSE,
    HOST_RESOLUTION_FAILURE,
    INTERNAL_ERROR,
    INVALID_URL_FORMAT,
    NETWORK_RECEIVE_ERROR,
    NETWORK_SEND_FAILURE,
    OPERATION_TIMEDOUT,
    PROXY_RESOLUTION_FAILURE,
    SSL_CONNECT_ERROR,
    SSL_LOCAL_CERTIFICATE_ERROR,
    SSL_REMOTE_CERTIFICATE_ERROR,
    SSL_CACERT_ERROR,
    GENERIC_SSL_ERROR,
    UNSUPPORTED_PROTOCOL,
    REQUEST_CANCELLED,
    TOO_MANY_REDIRECTS,
    UNKNOWN_ERROR = 1000,
};

class Error {
  public:
    ErrorCode code = ErrorCode::OK;
    std::string message{};

    Error() = default;

    Error(const std::int32_t& curl_code, std::string&& p_error_message) : code{getErrorCodeForCurlError(curl_code)}, message(std::move(p_error_message)) {}

    explicit operator bool() const {
        return code != ErrorCode::OK;
    }

  private:
    static ErrorCode getErrorCodeForCurlError(std::int32_t curl_code);
};

} // namespace cpr

#endif
