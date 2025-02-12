#ifndef CPR_STATUS_CODES
#define CPR_STATUS_CODES
namespace cpr {
namespace status {
// Information responses
constexpr long HTTP_CONTINUE = 100;
constexpr long HTTP_SWITCHING_PROTOCOL = 101;
constexpr long HTTP_PROCESSING = 102;
constexpr long HTTP_EARLY_HINTS = 103;
// Successful responses
constexpr long HTTP_OK = 200;
constexpr long HTTP_CREATED = 201;
constexpr long HTTP_ACCEPTED = 202;
constexpr long HTTP_NON_AUTHORITATIVE_INFORMATION = 203;
constexpr long HTTP_NO_CONTENT = 204;
constexpr long HTTP_RESET_CONTENT = 205;
constexpr long HTTP_PARTIAL_CONTENT = 206;
constexpr long HTTP_MULTI_STATUS = 207;
constexpr long HTTP_ALREADY_REPORTED = 208;
constexpr long HTTP_IM_USED = 226;
// Redirection messages
constexpr long HTTP_MULTIPLE_CHOICE = 300;
constexpr long HTTP_MOVED_PERMANENTLY = 301;
constexpr long HTTP_FOUND = 302;
constexpr long HTTP_SEE_OTHER = 303;
constexpr long HTTP_NOT_MODIFIED = 304;
constexpr long HTTP_USE_PROXY = 305;
constexpr long HTTP_UNUSED = 306;
constexpr long HTTP_TEMPORARY_REDIRECT = 307;
constexpr long HTTP_PERMANENT_REDIRECT = 308;
// Client error responses
constexpr long HTTP_BAD_REQUEST = 400;
constexpr long HTTP_UNAUTHORIZED = 401;
constexpr long HTTP_PAYMENT_REQUIRED = 402;
constexpr long HTTP_FORBIDDEN = 403;
constexpr long HTTP_NOT_FOUND = 404;
constexpr long HTTP_METHOD_NOT_ALLOWED = 405;
constexpr long HTTP_NOT_ACCEPTABLE = 406;
constexpr long HTTP_PROXY_AUTHENTICATION_REQUIRED = 407;
constexpr long HTTP_REQUEST_TIMEOUT = 408;
constexpr long HTTP_CONFLICT = 409;
constexpr long HTTP_GONE = 410;
constexpr long HTTP_LENGTH_REQUIRED = 411;
constexpr long HTTP_PRECONDITION_FAILED = 412;
constexpr long HTTP_PAYLOAD_TOO_LARGE = 413;
constexpr long HTTP_URI_TOO_LONG = 414;
constexpr long HTTP_UNSUPPORTED_MEDIA_TYPE = 415;
constexpr long HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416;
constexpr long HTTP_EXPECTATION_FAILED = 417;
constexpr long HTTP_IM_A_TEAPOT = 418;
constexpr long HTTP_MISDIRECTED_REQUEST = 421;
constexpr long HTTP_UNPROCESSABLE_ENTITY = 422;
constexpr long HTTP_LOCKED = 423;
constexpr long HTTP_FAILED_DEPENDENCY = 424;
constexpr long HTTP_TOO_EARLY = 425;
constexpr long HTTP_UPGRADE_REQUIRED = 426;
constexpr long HTTP_PRECONDITION_REQUIRED = 428;
constexpr long HTTP_TOO_MANY_REQUESTS = 429;
constexpr long HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE = 431;
constexpr long HTTP_UNAVAILABLE_FOR_LEGAL_REASONS = 451;
// Server response errors
constexpr long HTTP_INTERNAL_SERVER_ERROR = 500;
constexpr long HTTP_NOT_IMPLEMENTED = 501;
constexpr long HTTP_BAD_GATEWAY = 502;
constexpr long HTTP_SERVICE_UNAVAILABLE = 503;
constexpr long HTTP_GATEWAY_TIMEOUT = 504;
constexpr long HTTP_HTTP_VERSION_NOT_SUPPORTED = 505;
constexpr long HTTP_VARIANT_ALSO_NEGOTIATES = 506;
constexpr long HTTP_INSUFFICIENT_STORAGE = 507;
constexpr long HTTP_LOOP_DETECTED = 508;
constexpr long HTTP_NOT_EXTENDED = 510;
constexpr long HTTP_NETWORK_AUTHENTICATION_REQUIRED = 511;

constexpr long INFO_CODE_OFFSET = 100;
constexpr long SUCCESS_CODE_OFFSET = 200;
constexpr long REDIRECT_CODE_OFFSET = 300;
constexpr long CLIENT_ERROR_CODE_OFFSET = 400;
constexpr long SERVER_ERROR_CODE_OFFSET = 500;
constexpr long MISC_CODE_OFFSET = 600;

constexpr bool is_informational(const long code) {
    return (code >= INFO_CODE_OFFSET && code < SUCCESS_CODE_OFFSET);
}
constexpr bool is_success(const long code) {
    return (code >= SUCCESS_CODE_OFFSET && code < REDIRECT_CODE_OFFSET);
}
constexpr bool is_redirect(const long code) {
    return (code >= REDIRECT_CODE_OFFSET && code < CLIENT_ERROR_CODE_OFFSET);
}
constexpr bool is_client_error(const long code) {
    return (code >= CLIENT_ERROR_CODE_OFFSET && code < SERVER_ERROR_CODE_OFFSET);
}
constexpr bool is_server_error(const long code) {
    return (code >= SERVER_ERROR_CODE_OFFSET && code < MISC_CODE_OFFSET);
}

} // namespace status
} // namespace cpr
#endif
