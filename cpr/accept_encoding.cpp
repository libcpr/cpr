#include "cpr/accept_encoding.h"

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>

namespace cpr {

AcceptEncoding::AcceptEncoding(const std::initializer_list<AcceptEncodingMethods>& methods) {
    methods_.clear();
    std::transform(methods.begin(), methods.end(), std::inserter(methods_, methods_.begin()), [&](cpr::AcceptEncodingMethods method) { return cpr::AcceptEncodingMethodsStringMap.at(method); });
}

AcceptEncoding::AcceptEncoding(const std::initializer_list<std::string>& string_methods) : methods_{string_methods} {}

bool AcceptEncoding::empty() const noexcept {
    return methods_.empty();
}

const std::string AcceptEncoding::getString() const {
    return std::accumulate(std::next(methods_.begin()), methods_.end(), *methods_.begin(), [](std::string a, std::string b) { return std::move(a) + ", " + std::move(b); });
}

[[nodiscard]] bool AcceptEncoding::disabled() const {
    if (methods_.find(cpr::AcceptEncodingMethodsStringMap.at(AcceptEncodingMethods::disabled)) != methods_.end()) {
        if (methods_.size() != 1) {
            throw std::invalid_argument("AcceptEncoding does not accept any other values if 'disabled' is present. You set the following encodings: " + getString());
        }
        return true;
    }
    return false;
}

} // namespace cpr
