#ifndef CPR_HTTP_VERSION_H
#define CPR_HTTP_VERSION_H

#include <curl/curlver.h>

namespace cpr {
enum class HttpVersionCode {
    /**
     * Let libcurl decide which version is the best.
     **/
    VERSION_NONE,
    /**
     * Enforce HTTP 1.0 requests.
     **/
    VERSION_1_0,
    /**
     * Enforce HTTP 1.1 requests.
     **/
    VERSION_1_1,
#if LIBCURL_VERSION_NUM >= 0x072100 // 7.33.0
    /**
     * Attempt HTTP 2.0 requests.
     * Fallback to HTTP 1.1 if negotiation fails.
     **/
    VERSION_2_0,
#endif
#if LIBCURL_VERSION_NUM >= 0x072F00 // 7.47.0
    /**
     * Attempt HTTP 2.0 for HTTPS requests only.
     * Fallback to HTTP 1.1 if negotiation fails.
     * HTTP 1.1 will be used for HTTP connections.
     **/
    VERSION_2_0_TLS,
#endif
#if LIBCURL_VERSION_NUM >= 0x073100 // 7.49.0
    /**
     * Start HTTP 2.0 for HTTP requests.
     * Requires prior knowledge that the server supports HTTP 2.0.
     * For HTTPS requests we will negotiate the protocol version in the TLS handshake.
     **/
    VERSION_2_0_PRIOR_KNOWLEDGE,
#endif
#if LIBCURL_VERSION_NUM >= 0x074200 // 7.66.0
    /**
     * Attempt HTTP 3.0 requests.
     * Requires prior knowledge that the server supports HTTP 3.0 since there is no gracefully downgrade.
     * Fallback to HTTP 1.1 if negotiation fails.
     **/
    VERSION_3_0
#endif
};

class HttpVersion {
  public:
    /**
     * The HTTP version that should be used by libcurl when initiating a HTTP(S) connection.
     * Default: HttpVersionCode::VERSION_NONE
     **/
    HttpVersionCode code = HttpVersionCode::VERSION_NONE;

    HttpVersion() = default;
    explicit HttpVersion(HttpVersionCode _code) : code(_code) {}
};

} // namespace cpr

#endif
