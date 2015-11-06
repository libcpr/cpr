#ifndef CPR_ERROR_H
#define CPR_ERROR_H

#include <string>

#include "cprtypes.h"
#include "defines.h"

namespace cpr {

enum class ErrorCode {
    OK = 0,
    CONNECTION_FAILURE,
    EMPTY_RESPONSE,
    GENERIC_HTTP2_FAILURE, //really not sure about this one
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
    TOO_MANY_REDIRECTS,
    UNSUPPORTED_PROTOCOL,
    UNKNOWN_ERROR = 1000,
};

ErrorCode getErrorCodeForCurlError(int curlCode); //int so we don't have to include curl.h

class Error {
public:
    Error()
        : code{ErrorCode::OK}, message{""} {}

    template <typename ErrorCodeType, typename TextType>
    Error(ErrorCode& p_error_code, TextType&& p_error_message)
        : code{p_error_code}, message{CPR_FWD(p_error_message)} {}


    ErrorCode code;
    std::string message;

    //allow easy checking of errors with:
    //   if(error) { do something; }
    explicit operator bool() const {
        return code != ErrorCode::OK;
    }
};

} // namespace cpr

#endif
