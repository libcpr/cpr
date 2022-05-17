#ifndef CPR_ACCEPT_ENCODING_H
#define CPR_ACCEPT_ENCODING_H

#include <curl/curlver.h>
#include <initializer_list>
#include <map>
#include <string>
#include <vector>

namespace cpr {

enum class AcceptEncodingMethods {
    identity,
    deflate,
    zlib,
    gzip,
};

static const std::map<AcceptEncodingMethods, std::string> AcceptEncodingMethodsStringMap{{AcceptEncodingMethods::identity, "identity"}, {AcceptEncodingMethods::deflate, "deflate"}, {AcceptEncodingMethods::zlib, "zlib"}, {AcceptEncodingMethods::gzip, "gzip"}};

class AcceptEncoding {
  public:
    AcceptEncoding() = default;
    AcceptEncoding(const std::initializer_list<AcceptEncodingMethods>& methods);
    AcceptEncoding(const std::initializer_list<std::string>& methods);

    bool empty() const noexcept;
    const std::string getString() const;

  private:
    std::vector<std::string> methods_;
};

} // namespace cpr

#endif
