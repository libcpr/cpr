#include "cpr/response.h"

namespace cpr {
Response::Response(std::shared_ptr<CurlHolder> curl, std::string&& p_text,
                   std::string&& p_header_string, Cookies&& p_cookies = Cookies{},
                   Error&& p_error = Error{})
        : curl_(std::move(curl)), text(std::move(p_text)), cookies(std::move(p_cookies)),
          error(std::move(p_error)) {
    header = cpr::util::parseHeader(p_header_string, &status_line, &reason);
    assert(curl_);
    assert(curl_->handle);
    curl_easy_getinfo(curl_->handle, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_getinfo(curl_->handle, CURLINFO_TOTAL_TIME, &elapsed);
    char* url_string{nullptr};
    curl_easy_getinfo(curl_->handle, CURLINFO_EFFECTIVE_URL, &url_string);
    url = Url(url_string);
#if LIBCURL_VERSION_NUM >= 0x073700
    curl_easy_getinfo(curl_->handle, CURLINFO_SIZE_DOWNLOAD_T, &downloaded_bytes);
    curl_easy_getinfo(curl_->handle, CURLINFO_SIZE_UPLOAD_T, &uploaded_bytes);
#else
    double downloaded_bytes_double, uploaded_bytes_double;
    curl_easy_getinfo(curl_->handle, CURLINFO_SIZE_DOWNLOAD, &downloaded_bytes_double);
    curl_easy_getinfo(curl_->handle, CURLINFO_SIZE_UPLOAD, &uploaded_bytes_double);
    downloaded_bytes = downloaded_bytes_double;
    uploaded_bytes = uploaded_bytes_double;
#endif
    curl_easy_getinfo(curl_->handle, CURLINFO_REDIRECT_COUNT, &redirect_count);
}

std::vector<std::string> Response::GetCertInfo() {
    assert(curl_);
    assert(curl_->handle);
    curl_certinfo* ci{nullptr};
    curl_easy_getinfo(curl_->handle, CURLINFO_CERTINFO, &ci);

    std::vector<std::string> info;
    info.resize(ci->num_of_certs);
    for (int i = 0; i < ci->num_of_certs; i++) {
        // No way around here.
        // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        info[i] = std::string{ci->certinfo[i]->data};
    }

    return info;
}
} // namespace cpr
