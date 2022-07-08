#ifndef CPR_RESPONSE_H
#define CPR_RESPONSE_H

#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "cpr/cert_info.h"
#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/error.h"
#include "cpr/ssl_options.h"
#include "cpr/util.h"

namespace cpr {

class Response {
  private:
    std::shared_ptr<CurlHolder> curl_{nullptr};

  public:
    // Ignored here since libcurl uses a long for this.
    // NOLINTNEXTLINE(google-runtime-int)
    long status_code{};
    std::string text{};
    Header header{};
    Url url{};
    double elapsed{};
    Cookies cookies{};
    Error error{};
    std::string raw_header{};
    std::string status_line{};
    std::string reason{};
    cpr_off_t uploaded_bytes{};
    cpr_off_t downloaded_bytes{};
    // Ignored here since libcurl uses a long for this.
    // NOLINTNEXTLINE(google-runtime-int)
    long redirect_count{};

    Response() = default;
    Response(std::shared_ptr<CurlHolder> curl, std::string&& p_text, std::string&& p_header_string, Cookies&& p_cookies, Error&& p_error);
    std::vector<CertInfo> GetCertInfos();
    Response(const Response& other) = default;
    Response(Response&& old) noexcept = default;
    ~Response() noexcept = default;

    Response& operator=(Response&& old) noexcept = default;
    Response& operator=(const Response& other) = default;
};
} // namespace cpr

#endif
