#ifndef CPR_RESPONSE_H
#define CPR_RESPONSE_H

#include <cstdint>
#include <string>

#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/error.h"
#include <utility>

namespace cpr {

class Response {
  public:
    Response() = default;

    Response(const std::int32_t& p_status_code, std::string&& p_text, Header&& p_header,
             Url&& p_url, const double& p_elapsed, Cookies&& p_cookies = Cookies{},
             Error&& p_error = Error{}, std::string&& p_raw_header = "",
             std::string&& p_status_line = "", std::string&& p_reason = "",
             const double& p_uploaded_bytes = 0, const double& p_downloaded_bytes = 0)
            : status_code{p_status_code}, text(std::move(p_text)), header(std::move(p_header)),
              url(std::move(p_url)), elapsed{p_elapsed}, cookies(std::move(p_cookies)),
              error(std::move(p_error)), raw_header(std::move(p_raw_header)),
              status_line(std::move(p_status_line)), reason(std::move(p_reason)),
              uploaded_bytes{p_uploaded_bytes}, downloaded_bytes{p_downloaded_bytes} {}

    std::int32_t status_code;
    std::string text;
    Header header;
    Url url;
    double elapsed;
    Cookies cookies;
    Error error;
    std::string raw_header;
    std::string status_line;
    std::string reason;
    double uploaded_bytes;
    double downloaded_bytes;
};

} // namespace cpr

#endif
