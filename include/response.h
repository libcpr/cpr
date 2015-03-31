#ifndef _CPR_RESPONSE_H_
#define _CPR_RESPONSE_H_

#include <string>

#include "cprtypes.h"


class Response {
  public:
    Response(long status_code, std::string text, Header header, Url url, double elapsed)
        : status_code{status_code}, text{text}, header{header}, url{url}, elapsed{elapsed} {};

    long status_code;
    std::string text;
    Header header;
    Url url;
    double elapsed;
};

#endif
