#ifndef CPR_ACCEPT_ENCODING_H
#define CPR_ACCEPT_ENCODING_H

#include <curl/curlver.h>
#include <initializer_list>
#include <map>
#include <string>
#include <unordered_set>

namespace cpr {

enum class AcceptEncodingMethods {
    identity,
    deflate,
    zlib,
    gzip,
    disabled,
};

// NOLINTNEXTLINE(cert-err58-cpp)
static const std::map<AcceptEncodingMethods, std::string> AcceptEncodingMethodsStringMap{{AcceptEncodingMethods::identity, "identity"}, {AcceptEncodingMethods::deflate, "deflate"}, {AcceptEncodingMethods::zlib, "zlib"}, {AcceptEncodingMethods::gzip, "gzip"}, {AcceptEncodingMethods::disabled, "disabled"}};

class AcceptEncoding {
  public:
    AcceptEncoding() = default;
    // NOLINTNEXTLINE(google-explicit-constructor)
    AcceptEncoding(const std::initializer_list<AcceptEncodingMethods>& methods);
    // NOLINTNEXTLINE(google-explicit-constructor)
    AcceptEncoding(const std::initializer_list<std::string>& methods);

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const std::string getString() const;
    [[nodiscard]] bool disabled() const;

  private:
    std::unordered_set<std::string> methods_;
};

} // namespace cpr

#endif
