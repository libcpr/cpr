#include "cpr/accept_encoding.h"

#include <algorithm>
#include <iterator>
#include <numeric>

namespace cpr {

AcceptEncoding::AcceptEncoding(const std::initializer_list<AcceptEncodingMethods>& methods) {
    methods_.clear();
    std::transform(methods.begin(), methods.end(), std::back_inserter(methods_), [&](cpr::AcceptEncodingMethods method) { return cpr::AcceptEncodingMethodsStringMap.at(method); });
}

AcceptEncoding::AcceptEncoding(const std::initializer_list<std::string>& string_methods) : methods_{string_methods} {}

bool AcceptEncoding::empty() const noexcept {
    return methods_.empty();
}

const std::string AcceptEncoding::getString() const {
    std::string methodsString = std::accumulate(std::next(methods_.begin()), methods_.end(), methods_[0], [](std::string a, std::string b) { return std::move(a) + ", " + std::move(b); });
    return methodsString;
}

} // namespace cpr
