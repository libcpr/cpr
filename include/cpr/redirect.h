#ifndef CPR_REDIRECT_H
#define CPR_REDIRECT_H

#include <cstdint>

namespace cpr {
enum class PostRedirectFlags : uint8_t {
    /**
     * Respect RFC 7231 (section 6.4.2 to 6.4.4).
     * Same as CURL_REDIR_POST_301 (https://curl.se/libcurl/c/CURLOPT_POSTREDIR.html).
     **/
    POST_301 = 0x1 << 0,
    /**
     * Maintain the request method after a 302 redirect.
     * Same as CURL_REDIR_POST_302 (https://curl.se/libcurl/c/CURLOPT_POSTREDIR.html).
     **/
    POST_302 = 0x1 << 1,
    /**
     * Maintain the request method after a 303 redirect.
     * Same as CURL_REDIR_POST_303 (https://curl.se/libcurl/c/CURLOPT_POSTREDIR.html).
     **/
    POST_303 = 0x1 << 2,
    /**
     * Default value.
     * Convenience option to enable all flags.
     * Same as CURL_REDIR_POST_ALL (https://curl.se/libcurl/c/CURLOPT_POSTREDIR.html).
     **/
    POST_ALL = POST_301 | POST_302 | POST_303,
    /**
     * * Convenience option to disable all flags.
     **/
    NONE = 0x0
};

PostRedirectFlags operator|(PostRedirectFlags lhs, PostRedirectFlags rhs);
PostRedirectFlags operator&(PostRedirectFlags lhs, PostRedirectFlags rhs);
PostRedirectFlags operator^(PostRedirectFlags lhs, PostRedirectFlags rhs);
PostRedirectFlags operator~(PostRedirectFlags flag);
PostRedirectFlags& operator|=(PostRedirectFlags& lhs, PostRedirectFlags rhs);
PostRedirectFlags& operator&=(PostRedirectFlags& lhs, PostRedirectFlags rhs);
PostRedirectFlags& operator^=(PostRedirectFlags& lhs, PostRedirectFlags rhs);
bool any(PostRedirectFlags flag);

class Redirect {
  public:
    /**
     * The maximum number of redirects to follow.
     * 0: Refuse any redirects.
     * -1: Infinite number of redirects.
     * Default: 50
     * https://curl.se/libcurl/c/CURLOPT_MAXREDIRS.html
     **/
    // NOLINTNEXTLINE (google-runtime-int)
    long maximum{50L};
    /**
     * Follow 3xx redirects.
     * Default: true
     * https://curl.se/libcurl/c/CURLOPT_FOLLOWLOCATION.html
     **/
    bool follow{true};
    /**
     * Continue to send authentication (user+password) credentials when following locations, even when hostname changed.
     * Default: false
     * https://curl.se/libcurl/c/CURLOPT_UNRESTRICTED_AUTH.html
     **/
    bool cont_send_cred{false};
    /**
     * Flags to control how to act after a redirect for a post request.
     * Default: POST_ALL
     **/
    PostRedirectFlags post_flags{PostRedirectFlags::POST_ALL};

    Redirect() = default;
    // NOLINTNEXTLINE (google-runtime-int)
    Redirect(long p_maximum, bool p_follow, bool p_cont_send_cred, PostRedirectFlags p_post_flags) : maximum(p_maximum), follow(p_follow), cont_send_cred(p_cont_send_cred), post_flags(p_post_flags){};
    // NOLINTNEXTLINE (google-runtime-int)
    explicit Redirect(long p_maximum) : maximum(p_maximum){};
    explicit Redirect(bool p_follow) : follow(p_follow){};
    Redirect(bool p_follow, bool p_cont_send_cred) : follow(p_follow), cont_send_cred(p_cont_send_cred){};
    explicit Redirect(PostRedirectFlags p_post_flags) : post_flags(p_post_flags){};
};
} // namespace cpr

#endif
