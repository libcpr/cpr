#ifndef CPR_RESPONSE_H
#define CPR_RESPONSE_H

#include <string>

#include "cookies.h"
#include "cprtypes.h"
#include "defines.h"

namespace cpr {
class Response {
  public:
    Response() = default;

    template <typename TextType, typename HeaderType, typename UrlType, typename CookiesType>
    Response(const long& p_status_code, TextType&& p_text, HeaderType&& p_header, UrlType&& p_url,
             const double& p_elapsed, CookiesType&& p_cookies = Cookies{})
            : status_code{p_status_code}, text{CPR_FWD(p_text)}, header{CPR_FWD(p_header)},
              url{CPR_FWD(p_url)}, elapsed{p_elapsed}, cookies{CPR_FWD(p_cookies)} {}

    long status_code;
    std::string text;
    Header header;
    Url url;
    double elapsed;
    Cookies cookies;
};

} // namespace cpr

#endif
