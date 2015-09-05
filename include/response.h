#ifndef CPR_RESPONSE_H
#define CPR_RESPONSE_H

#include <string>

#include "cookies.h"
#include "cprtypes.h"


class Response {
  public:
    Response() = default;
    Response(const long& status_code, const std::string& text, const Header& header, const Url& url,
             const double& elapsed)
        : status_code{status_code}, text{text}, header{header}, url{url}, elapsed{elapsed} {};
    Response(const long& status_code, const std::string& text, const Header& header, const Url& url,
             const double& elapsed, const Cookies& cookies)
        : status_code{status_code}, text{text}, header{header}, url{url}, elapsed{elapsed},
          cookies{cookies} {};

    long status_code;
    std::string text;
    Header header;
    Url url;
    double elapsed;
    Cookies cookies;
};

#endif
