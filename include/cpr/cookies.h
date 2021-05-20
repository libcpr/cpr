#ifndef CPR_COOKIES_H
#define CPR_COOKIES_H

#include "cpr/curlholder.h"
#include <initializer_list>
#include <map>
#include <sstream>
#include <string>

namespace cpr {

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
    Cookies(bool p_encode = true) : encode(p_encode) {}
    Cookies(const std::initializer_list<std::pair<const std::string, std::string>>& pairs,
            bool p_encode = true)
            : encode(p_encode), map_{pairs} {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Cookies(const std::map<std::string, std::string>& map, bool p_encode = true)
            : encode(p_encode), map_{map} {}

    std::string& operator[](const std::string& key);
    std::string GetEncoded(const CurlHolder& holder) const;

    using iterator = std::map<std::string, std::string>::iterator;
    using const_iterator = std::map<std::string, std::string>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

  private:
    std::map<std::string, std::string> map_;
};

} // namespace cpr

#endif
