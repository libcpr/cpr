#include "cookies.h"

#include "util.h"

namespace cpr {

Cookies::Cookies(const std::initializer_list<std::pair<const std::string, std::string>>& pairs)
        : map_{pairs} {}

std::string Cookies::GetEncoded() const {
    std::stringstream stream;
    for (const auto& item : map_) {
        stream << cpr::util::urlEncode(item.first) << "=" << cpr::util::urlEncode(item.second)
               << "; ";
    }
    return stream.str();
}

std::string& Cookies::operator[](const std::string& key) {
    return map_[key];
}

} // namespace cpr
