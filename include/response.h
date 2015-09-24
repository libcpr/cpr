#ifndef CPR_RESPONSE_H
#define CPR_RESPONSE_H

#include <string>

#include "cookies.h"
#include "cprtypes.h"

namespace cpr {
class Response {
  public:
    Response() = default;
    template <typename TextType, typename HeaderType, typename UrlType>
    Response(const long& status_code, TextType&& text, HeaderType&& header, UrlType&& url,
             const double& elapsed)
            : status_code{status_code}, text{CPR_FWD(text)}, header{CPR_FWD(header)},
              url{CPR_FWD(url)}, elapsed{elapsed} {}
    template <typename TextType, typename HeaderType, typename UrlType, typename CookiesType>
    Response(const long& status_code, TextType&& text, HeaderType&& header, UrlType&& url,
             const double& elapsed, CookiesType&& cookies)
            : status_code{status_code}, text{CPR_FWD(text)}, header{CPR_FWD(header)},
              url{CPR_FWD(url)}, elapsed{elapsed}, cookies{CPR_FWD(cookies)} {}

    long status_code;
    std::string text;
    Header header;
    Url url;
    double elapsed;
    Cookies cookies;
};

} // namespace cpr

#endif
