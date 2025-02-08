#include "cpr/cookies.h"
#include "cpr/curlholder.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

namespace cpr {
const std::string& Cookie::GetDomain() const {
    return domain_;
}

bool Cookie::IsIncludingSubdomains() const {
    return includeSubdomains_;
}

const std::string& Cookie::GetPath() const {
    return path_;
}

bool Cookie::IsHttpsOnly() const {
    return httpsOnly_;
}

std::chrono::system_clock::time_point Cookie::GetExpires() const {
    return expires_;
}

std::string Cookie::GetExpiresString() const {
    std::stringstream ss;
    std::tm tm{};
    const std::time_t tt = std::chrono::system_clock::to_time_t(expires_);
#ifdef _WIN32
    gmtime_s(&tm, &tt);
#else
    // NOLINTNEXTLINE(misc-include-cleaner,cert-err33-c) False positive since <ctime> is included. Also ignore the ret value here.
    gmtime_r(&tt, &tm);
#endif
    ss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
    return ss.str();
}

const std::string& Cookie::GetName() const {
    return name_;
}

const std::string& Cookie::GetValue() const {
    return value_;
}

std::string Cookies::GetEncoded(const CurlHolder& holder) const {
    std::stringstream stream;
    for (const cpr::Cookie& item : cookies_) {
        // Depending on if encoding is set to "true", we will URL-encode cookies
        stream << (encode ? std::string_view{holder.urlEncode(item.GetName())} : std::string_view{item.GetName()}) << "=";

        // special case version 1 cookies, which can be distinguished by
        // beginning and trailing quotes
        if (!item.GetValue().empty() && item.GetValue().front() == '"' && item.GetValue().back() == '"') {
            stream << item.GetValue();
        } else {
            // Depending on if encoding is set to "true", we will URL-encode cookies
            stream << (encode ? std::string_view{holder.urlEncode(item.GetValue())} : std::string_view{item.GetValue()});
        }
        stream << "; ";
    }
    return stream.str();
}

cpr::Cookie& Cookies::operator[](size_t pos) {
    return cookies_[pos];
}

Cookies::iterator Cookies::begin() {
    return cookies_.begin();
}

Cookies::iterator Cookies::end() {
    return cookies_.end();
}

Cookies::const_iterator Cookies::begin() const {
    return cookies_.begin();
}

Cookies::const_iterator Cookies::end() const {
    return cookies_.end();
}

Cookies::const_iterator Cookies::cbegin() const {
    return cookies_.cbegin();
}

Cookies::const_iterator Cookies::cend() const {
    return cookies_.cend();
}

void Cookies::emplace_back(const Cookie& str) {
    cookies_.emplace_back(str);
}

bool Cookies::empty() const {
    return cookies_.empty();
}

void Cookies::push_back(const Cookie& str) {
    cookies_.push_back(str);
}

void Cookies::pop_back() {
    cookies_.pop_back();
}

} // namespace cpr
