#ifndef CPR_RESPONSE_H
#define CPR_RESPONSE_H

#include <cassert>
#include <cstdint>
#include <curl/curl.h>
#include <memory>
#include <string>

#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/error.h"
#include "cpr/util.h"
#include <utility>

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
             Cookies&& p_cookies = Cookies{}, Error&& p_error = Error{})
            : curl_(std::move(curl)), text(std::move(p_text)), cookies(std::move(p_cookies)),
              error(std::move(p_error)) {
        header = cpr::util::parseHeader(p_header_string, &status_line, &reason);
        assert(curl_->handle);
        curl_easy_getinfo(curl_->handle, CURLINFO_RESPONSE_CODE, &status_code);
        curl_easy_getinfo(curl_->handle, CURLINFO_TOTAL_TIME, &elapsed);
        char* url_string{nullptr};
        curl_easy_getinfo(curl_->handle, CURLINFO_EFFECTIVE_URL, &url_string);
        url = Url(url_string);
        curl_easy_getinfo(curl_->handle, CURLINFO_SIZE_DOWNLOAD_T, &downloaded_bytes);
        curl_easy_getinfo(curl_->handle, CURLINFO_SIZE_UPLOAD_T, &uploaded_bytes);
        curl_easy_getinfo(curl_->handle, CURLINFO_REDIRECT_COUNT, &redirect_count);
    }
};
} // namespace cpr

#endif
