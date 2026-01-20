#ifndef CPR_STATUS_CODES
#define CPR_STATUS_CODES
namespace cpr::status {
// Information responses
inline constexpr long HTTP_CONTINUE = 100;
inline constexpr long HTTP_SWITCHING_PROTOCOL = 101;
inline constexpr long HTTP_PROCESSING = 102;
inline constexpr long HTTP_EARLY_HINTS = 103;
// Successful responses
inline constexpr long HTTP_OK = 200;
inline constexpr long HTTP_CREATED = 201;
inline constexpr long HTTP_ACCEPTED = 202;
inline constexpr long HTTP_NON_AUTHORITATIVE_INFORMATION = 203;
inline constexpr long HTTP_NO_CONTENT = 204;
inline constexpr long HTTP_RESET_CONTENT = 205;
inline constexpr long HTTP_PARTIAL_CONTENT = 206;
inline constexpr long HTTP_MULTI_STATUS = 207;
inline constexpr long HTTP_ALREADY_REPORTED = 208;
inline constexpr long HTTP_IM_USED = 226;
// Redirection messages
inline constexpr long HTTP_MULTIPLE_CHOICE = 300;
inline constexpr long HTTP_MOVED_PERMANENTLY = 301;
inline constexpr long HTTP_FOUND = 302;
inline constexpr long HTTP_SEE_OTHER = 303;
inline constexpr long HTTP_NOT_MODIFIED = 304;
inline constexpr long HTTP_USE_PROXY = 305;
inline constexpr long HTTP_UNUSED = 306;
inline constexpr long HTTP_TEMPORARY_REDIRECT = 307;
inline constexpr long HTTP_PERMANENT_REDIRECT = 308;
// Client error responses
inline constexpr long HTTP_BAD_REQUEST = 400;
inline constexpr long HTTP_UNAUTHORIZED = 401;
inline constexpr long HTTP_PAYMENT_REQUIRED = 402;
inline constexpr long HTTP_FORBIDDEN = 403;
inline constexpr long HTTP_NOT_FOUND = 404;
inline constexpr long HTTP_METHOD_NOT_ALLOWED = 405;
inline constexpr long HTTP_NOT_ACCEPTABLE = 406;
inline constexpr long HTTP_PROXY_AUTHENTICATION_REQUIRED = 407;
inline constexpr long HTTP_REQUEST_TIMEOUT = 408;
inline constexpr long HTTP_CONFLICT = 409;
inline constexpr long HTTP_GONE = 410;
inline constexpr long HTTP_LENGTH_REQUIRED = 411;
inline constexpr long HTTP_PRECONDITION_FAILED = 412;
inline constexpr long HTTP_PAYLOAD_TOO_LARGE = 413;
inline constexpr long HTTP_URI_TOO_LONG = 414;
inline constexpr long HTTP_UNSUPPORTED_MEDIA_TYPE = 415;
inline constexpr long HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416;
inline constexpr long HTTP_EXPECTATION_FAILED = 417;
inline constexpr long HTTP_IM_A_TEAPOT = 418;
inline constexpr long HTTP_MISDIRECTED_REQUEST = 421;
inline constexpr long HTTP_UNPROCESSABLE_ENTITY = 422;
inline constexpr long HTTP_LOCKED = 423;
inline constexpr long HTTP_FAILED_DEPENDENCY = 424;
inline constexpr long HTTP_TOO_EARLY = 425;
inline constexpr long HTTP_UPGRADE_REQUIRED = 426;
inline constexpr long HTTP_PRECONDITION_REQUIRED = 428;
inline constexpr long HTTP_TOO_MANY_REQUESTS = 429;
inline constexpr long HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE = 431;
inline constexpr long HTTP_UNAVAILABLE_FOR_LEGAL_REASONS = 451;
// Server response errors
inline constexpr long HTTP_INTERNAL_SERVER_ERROR = 500;
inline constexpr long HTTP_NOT_IMPLEMENTED = 501;
inline constexpr long HTTP_BAD_GATEWAY = 502;
inline constexpr long HTTP_SERVICE_UNAVAILABLE = 503;
inline constexpr long HTTP_GATEWAY_TIMEOUT = 504;
inline constexpr long HTTP_HTTP_VERSION_NOT_SUPPORTED = 505;
inline constexpr long HTTP_VARIANT_ALSO_NEGOTIATES = 506;
inline constexpr long HTTP_INSUFFICIENT_STORAGE = 507;
inline constexpr long HTTP_LOOP_DETECTED = 508;
inline constexpr long HTTP_NOT_EXTENDED = 510;
inline constexpr long HTTP_NETWORK_AUTHENTICATION_REQUIRED = 511;

inline constexpr long INFO_CODE_OFFSET = 100;
inline constexpr long SUCCESS_CODE_OFFSET = 200;
inline constexpr long REDIRECT_CODE_OFFSET = 300;
inline constexpr long CLIENT_ERROR_CODE_OFFSET = 400;
inline constexpr long SERVER_ERROR_CODE_OFFSET = 500;
inline constexpr long MISC_CODE_OFFSET = 600;

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
} // namespace cpr::status
#endif
