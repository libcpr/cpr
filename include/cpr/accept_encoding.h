#ifndef CPR_ACCEPT_ENCODING_H
#define CPR_ACCEPT_ENCODING_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <initializer_list>
#include <map>
#include <string>
#include <unordered_set>
#endif

#include <cstdint>
#include <curl/curlver.h>
#include <sys/types.h>

namespace cpr {

EXPORT_CPR enum class AcceptEncodingMethods : uint8_t {
    identity,
    deflate,
    zlib,
    gzip,
    disabled,
};

// NOLINTNEXTLINE(cert-err58-cpp)
EXPORT_CPR inline const std::map<AcceptEncodingMethods, std::string> AcceptEncodingMethodsStringMap{{AcceptEncodingMethods::identity, "identity"}, {AcceptEncodingMethods::deflate, "deflate"}, {AcceptEncodingMethods::zlib, "zlib"}, {AcceptEncodingMethods::gzip, "gzip"}, {AcceptEncodingMethods::disabled, "disabled"}};

EXPORT_CPR class AcceptEncoding {
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
