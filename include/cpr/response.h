#ifndef CPR_RESPONSE_H
#define CPR_RESPONSE_H

#include <cassert>
#include <cstdint>
#include <curl/curl.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/error.h"
#include "cpr/ssl_options.h"
#include "cpr/util.h"

namespace cpr {

class Response {
  private:
    std::shared_ptr<CurlHolder> curl_;

  public:
    long status_code;
    std::string text;
    Header header;
    Url url;
    double elapsed;
    Cookies cookies;
    Error error;
    std::string raw_header;
    std::string status_line;
    std::string reason;
    cpr_off_t uploaded_bytes;
    cpr_off_t downloaded_bytes;
    long redirect_count;

  public:
    Response() = default;
    Response(std::shared_ptr<CurlHolder> curl, std::string&& p_text, std::string&& p_header_string,
             Cookies&& p_cookies, Error&& p_error);
    std::vector<std::string> GetCertInfo();
};
} // namespace cpr

#endif
