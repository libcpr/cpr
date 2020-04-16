#ifndef _CPR_STATUS_CODES
#define _CPR_STATUS_CODES
#include <cstdint>
namespace cpr {
namespace status {
// Information responses
constexpr std::int32_t HTTP_CONTINUE = 100;
constexpr std::int32_t HTTP_SWITCHING_PROTOCOL = 101;
constexpr std::int32_t HTTP_PROCESSING = 102;
constexpr std::int32_t HTTP_EARLY_HINTS = 103;
// Sucessful responses
constexpr std::int32_t HTTP_OK = 200;
constexpr std::int32_t HTTP_CREATED = 201;
constexpr std::int32_t HTTP_ACCEPTED = 202;
constexpr std::int32_t HTTP_NON_AUTHORITATIVE_INFORMATION = 203;
constexpr std::int32_t HTTP_NO_CONTENT = 204;
constexpr std::int32_t HTTP_RESET_CONTENT = 205;
constexpr std::int32_t HTTP_PARTIAL_CONTENT = 206;
constexpr std::int32_t HTTP_MULTI_STATUS = 207;
constexpr std::int32_t HTTP_ALREADY_REPORTED = 208;
constexpr std::int32_t HTTP_IM_USED = 226;
// Redirection messages
constexpr std::int32_t HTTP_MULTIPLE_CHOICE = 300;
constexpr std::int32_t HTTP_MOVED_PERMANENTLY = 301;
constexpr std::int32_t HTTP_FOUND = 302;
constexpr std::int32_t HTTP_SEE_OTHER = 303;
constexpr std::int32_t HTTP_NOT_MODIFIED = 304;
constexpr std::int32_t HTTP_USE_PROXY = 305;
constexpr std::int32_t HTTP_UNUSED = 306;
constexpr std::int32_t HTTP_TEMPORARY_REDIRECT = 307;
constexpr std::int32_t HTTP_PERMANENT_REDIRECT = 308;
// Client error responses
constexpr std::int32_t HTTP_BAD_REQUEST = 400;
constexpr std::int32_t HTTP_UNAUTHORIZED = 401;
constexpr std::int32_t HTTP_PAYMENT_REQUIRED = 402;
constexpr std::int32_t HTTP_FORBIDDEN = 403;
constexpr std::int32_t HTTP_NOT_FOUND = 404;
constexpr std::int32_t HTTP_METHOD_NOT_ALLOWED = 405;
constexpr std::int32_t HTTP_NOT_ACCEPTABLE = 406;
constexpr std::int32_t HTTP_PROXY_AUTHENTICATION_REQUIRED = 407;
constexpr std::int32_t HTTP_REQUEST_TIMEOUT = 408;
constexpr std::int32_t HTTP_CONFLICT = 409;
constexpr std::int32_t HTTP_GONE = 410;
constexpr std::int32_t HTTP_LENGTH_REQUIRED = 411;
constexpr std::int32_t HTTP_PRECONDITION_FAILED = 412;
constexpr std::int32_t HTTP_PAYLOAD_TOO_LARGE = 413;
constexpr std::int32_t HTTP_URI_TOO_LONG = 414;
constexpr std::int32_t HTTP_UNSUPPORTED_MEDIA_TYPE = 415;
constexpr std::int32_t HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416;
constexpr std::int32_t HTTP_EXPECTATION_FAILED = 417;
constexpr std::int32_t HTTP_IM_A_TEAPOT = 418;
constexpr std::int32_t HTTP_MISDIRECTED_REQUEST = 421;
constexpr std::int32_t HTTP_UNPROCESSABLE_ENTITY = 422;
constexpr std::int32_t HTTP_LOCKED = 423;
constexpr std::int32_t HTTP_FAILED_DEPENDENCY = 424;
constexpr std::int32_t HTTP_TOO_EARLY = 425;
constexpr std::int32_t HTTP_UPGRADE_REQUIRED = 426;
constexpr std::int32_t HTTP_PRECONDITION_REQUIRED = 428;
constexpr std::int32_t HTTP_TOO_MANY_REQUESTS = 429;
constexpr std::int32_t HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE = 431;
constexpr std::int32_t HTTP_UNAVAILABLE_FOR_LEGAL_REASONS = 451;
// Server response errors
constexpr std::int32_t HTTP_INTERNAL_SERVER_ERROR = 500;
constexpr std::int32_t HTTP_NOT_IMPLEMENTED = 501;
constexpr std::int32_t HTTP_BAD_GATEWAY = 502;
constexpr std::int32_t HTTP_SERVICE_UNAVAILABLE = 503;
constexpr std::int32_t HTTP_GATEWAY_TIMEOUT = 504;
constexpr std::int32_t HTTP_HTTP_VERSION_NOT_SUPPORTED = 505;
constexpr std::int32_t HTTP_VARIANT_ALSO_NEGOTIATES = 506;
constexpr std::int32_t HTTP_INSUFFICIENT_STORAGE = 507;
constexpr std::int32_t HTTP_LOOP_DETECTED = 508;
constexpr std::int32_t HTTP_NOT_EXTENDED = 510;
constexpr std::int32_t HTTP_NETWORK_AUTHENTICATION_REQUIRED = 511;

constexpr bool is_informational(const std::int32_t code) {
    return (code >= 100 && code < 200);
}
constexpr bool is_success(const std::int32_t code) {
    return (code >= 200 && code < 300);
}
constexpr bool is_redirect(const std::int32_t code) {
    return (code >= 300 && code < 400);
}
constexpr bool is_client_error(const std::int32_t code) {
    return (code >= 400 && code < 500);
}
constexpr bool is_server_error(const std::int32_t code) {
    return (code >= 500 && code < 600);
}

} // namespace status
} // namespace cpr
#endif