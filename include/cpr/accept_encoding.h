#ifndef CPR_ACCEPT_ENCODING_H
#define CPR_ACCEPT_ENCODING_H

#include <cstdint>
#include <curl/curlver.h>
#include <initializer_list>
#include <map>
#include <string>
#include <sys/types.h>
#include <unordered_set>

namespace cpr {

enum class AcceptEncodingMethods : uint8_t {
    identity,
    deflate,
    zlib,
    gzip,
    disabled,
};

// NOLINTNEXTLINE(cert-err58-cpp)
inline const std::map<AcceptEncodingMethods, std::string> AcceptEncodingMethodsStringMap{{AcceptEncodingMethods::identity, "identity"}, {AcceptEncodingMethods::deflate, "deflate"}, {AcceptEncodingMethods::zlib, "zlib"}, {AcceptEncodingMethods::gzip, "gzip"}, {AcceptEncodingMethods::disabled, "disabled"}};

class AcceptEncoding {
  public:
    AcceptEncoding() = default;
    AcceptEncoding(const std::initializer_list<AcceptEncodingMethods>& methods);
    AcceptEncoding(const std::initializer_list<std::string>& methods);

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const std::string getString() const;
    [[nodiscard]] bool disabled() const;

  private:
    std::unordered_set<std::string> methods_;
};

} // namespace cpr

#endif
