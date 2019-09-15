#ifndef _CPR_STATUS_CODES
#define _CPR_STATUS_CODES
#include <cstdint>
namespace cpr {
namespace status {
constexpr std::uint32_t HTTP_100_CONTINUE = 100;
constexpr std::uint32_t HTTP_101_SWITCHING_PROTOCOL = 101;
constexpr std::uint32_t HTTP_102_PROCESSING = 102;
constexpr std::uint32_t HTTP_103_EARLY_HINTS = 103;
constexpr std::uint32_t HTTP_200_OK = 200;
constexpr std::uint32_t HTTP_201_CREATED = 201;
constexpr std::uint32_t HTTP_202_ACCEPTED = 202;
constexpr std::uint32_t HTTP_203_NON_AUTHORITATIVE_INFORMATION = 203;
constexpr std::uint32_t HTTP_204_NO_CONTENT = 204;
constexpr std::uint32_t HTTP_205_RESET_CONTENT = 205;
constexpr std::uint32_t HTTP_206_PARTIAL_CONTENT = 206;
constexpr std::uint32_t HTTP_207_MULTI_STATUS = 207;
constexpr std::uint32_t HTTP_208_ALREADY_REPORTED = 208;
constexpr std::uint32_t HTTP_226_IM_USED = 226;
constexpr std::uint32_t HTTP_300_MULTIPLE_CHOICE = 300;
constexpr std::uint32_t HTTP_301_MOVED_PERMANENTLY = 301;
constexpr std::uint32_t HTTP_302_FOUND = 302;
constexpr std::uint32_t HTTP_303_SEE_OTHER = 303;
constexpr std::uint32_t HTTP_304_NOT_MODIFIED = 304;
constexpr std::uint32_t HTTP_305_USE_PROXY = 305;
constexpr std::uint32_t HTTP_306_UNUSED = 306;
constexpr std::uint32_t HTTP_307_TEMPORARY_REDIRECT = 307;
constexpr std::uint32_t HTTP_308_PERMANENT_REDIRECT = 308;
constexpr std::uint32_t HTTP_400_BAD_REQUEST = 400;
constexpr std::uint32_t HTTP_401_UNAUTHORIZED = 401;
constexpr std::uint32_t HTTP_402_PAYMENT_REQUIRED = 402;
constexpr std::uint32_t HTTP_403_FORBIDDEN = 403;
constexpr std::uint32_t HTTP_404_NOT_FOUND = 404;
constexpr std::uint32_t HTTP_405_METHOD_NOT_ALLOWED = 405;
constexpr std::uint32_t HTTP_406_NOT_ACCEPTABLE = 406;
constexpr std::uint32_t HTTP_407_PROXY_AUTHENTICATION_REQUIRED = 407;
constexpr std::uint32_t HTTP_408_REQUEST_TIMEOUT = 408;
constexpr std::uint32_t HTTP_409_CONFLICT = 409;
constexpr std::uint32_t HTTP_410_GONE = 410;
constexpr std::uint32_t HTTP_411_LENGTH_REQUIRED = 411;
constexpr std::uint32_t HTTP_412_PRECONDITION_FAILED = 412;
constexpr std::uint32_t HTTP_413_PAYLOAD_TOO_LARGE = 413;
constexpr std::uint32_t HTTP_414_URI_TOO_LONG = 414;
constexpr std::uint32_t HTTP_415_UNSUPPORTED_MEDIA_TYPE = 415;
constexpr std::uint32_t HTTP_416_REQUESTED_RANGE_NOT_SATISFIABLE = 416;
constexpr std::uint32_t HTTP_417_EXPECTATION_FAILED = 417;
constexpr std::uint32_t HTTP_418_IM_A_TEAPOT = 418;
constexpr std::uint32_t HTTP_421_MISDIRECTED_REQUEST = 421;
constexpr std::uint32_t HTTP_422_UNPROCESSABLE_ENTITY = 422;
constexpr std::uint32_t HTTP_423_LOCKED = 423;
constexpr std::uint32_t HTTP_424_FAILED_DEPENDENCY = 424;
constexpr std::uint32_t HTTP_425_TOO_EARLY = 425;
constexpr std::uint32_t HTTP_426_UPGRADE_REQUIRED = 426;
constexpr std::uint32_t HTTP_428_PRECONDITION_REQUIRED = 428;
constexpr std::uint32_t HTTP_429_TOO_MANY_REQUESTS = 429;
constexpr std::uint32_t HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431;
constexpr std::uint32_t HTTP_451_UNAVAILABLE_FOR_LEGAL_REASONS = 451;
constexpr std::uint32_t HTTP_500_INTERNAL_SERVER_ERROR = 500;
constexpr std::uint32_t HTTP_501_NOT_IMPLEMENTED = 501;
constexpr std::uint32_t HTTP_502_BAD_GATEWAY = 502;
constexpr std::uint32_t HTTP_503_SERVICE_UNAVAILABLE = 503;
constexpr std::uint32_t HTTP_504_GATEWAY_TIMEOUT = 504;
constexpr std::uint32_t HTTP_505_HTTP_VERSION_NOT_SUPPORTED = 505;
constexpr std::uint32_t HTTP_506_VARIANT_ALSO_NEGOTIATES = 506;
constexpr std::uint32_t HTTP_507_INSUFFICIENT_STORAGE = 507;
constexpr std::uint32_t HTTP_508_LOOP_DETECTED = 508;
constexpr std::uint32_t HTTP_510_NOT_EXTENDED = 510;
constexpr std::uint32_t HTTP_511_NETWORK_AUTHENTICATION_REQUIRED = 511;
} // namespace status
} // namespace cpr
#endif