#ifndef CPR_COOKIES_H
#define CPR_COOKIES_H

#include "cpr/curlholder.h"
#include <chrono>
#include <initializer_list>
#include <sstream>
#include <string>
#include <vector>

namespace cpr {
/**
 * EXPIRES_STRING_SIZE is an explicitly static and const variable that could be only accessed within the same namespace and is immutable.
 * To be used for "std::array", the expression must have a constant value, so EXPIRES_STRING_SIZE must be a const value.
 **/
static const std::size_t EXPIRES_STRING_SIZE = 100;

class Cookie {
  public:
    Cookie() = default;
    /**
     * Some notes for the default value used by expires:
     * std::chrono::system_clock::time_point::min() won't work on Windows due to the min, max clash there.
     * So we fall back to std::chrono::system_clock::from_time_t(0) for the minimum value here.
     **/
    Cookie(const std::string& name, const std::string& value, const std::string& domain = "", bool p_isIncludingSubdomains = false, const std::string& path = "/", bool p_isHttpsOnly = false, std::chrono::system_clock::time_point expires = std::chrono::system_clock::from_time_t(0)) : name_{name}, value_{value}, domain_{domain}, includeSubdomains_{p_isIncludingSubdomains}, path_{path}, httpsOnly_{p_isHttpsOnly}, expires_{expires} {};
    const std::string GetDomain() const;
    bool IsIncludingSubdomains() const;
    const std::string GetPath() const;
    bool IsHttpsOnly() const;
    const std::chrono::system_clock::time_point GetExpires() const;
    const std::string GetExpiresString() const;
    const std::string GetName() const;
    const std::string GetValue() const;

  private:
    std::string name_;
    std::string value_;
    std::string domain_;
    bool includeSubdomains_{};
    std::string path_;
    bool httpsOnly_{};
    /**
     * TODO: Update the implementation using `std::chrono::utc_clock` of C++20
     **/
    std::chrono::system_clock::time_point expires_{};
};

class Cookies {
  public:
    /**
     * Should we URL-encode cookies when making a request.
     * Based on RFC6265, it is recommended but not mandatory to encode cookies.
     *
     * -------
     * To maximize compatibility with user agents, servers that wish to
     * store arbitrary data in a cookie-value SHOULD encode that data, for
     * example, using Base64 [RFC4648].
     * -------
     * Source: RFC6265 (https://www.ietf.org/rfc/rfc6265.txt)
     **/
    bool encode{true};

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Cookies(bool p_encode = true) : encode{p_encode} {};
    Cookies(const std::initializer_list<cpr::Cookie>& cookies, bool p_encode = true) : encode{p_encode}, cookies_{cookies} {};
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Cookies(const cpr::Cookie& cookie, bool p_encode = true) : encode{p_encode}, cookies_{cookie} {};

    cpr::Cookie& operator[](size_t pos);
    const std::string GetEncoded(const CurlHolder& holder) const;

    using iterator = std::vector<cpr::Cookie>::iterator;
    using const_iterator = std::vector<cpr::Cookie>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    void emplace_back(const Cookie& str);
    void push_back(const Cookie& str);
    void pop_back();

  private:
    std::vector<cpr::Cookie> cookies_;
};

} // namespace cpr

#endif
