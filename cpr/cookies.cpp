#include "cookies.h"

#include "util.h"

namespace cpr {

Cookies::Cookies(const std::initializer_list<std::pair<const std::string, std::string>>& pairs)
        : map_{pairs} {}

std::string Cookies::GetEncoded() const {
    std::stringstream stream;
    for (const auto& item : map_) {
        // special case version 1 cookies, which can be distinguished by
        // beginning and trailing quotes
        if (!item.second.empty() && item.second.front() == '"' && item.second.back() == '"') {
            stream << cpr::util::urlEncode(item.first) << "=" << item.second << "; ";
        } else {
            stream << cpr::util::urlEncode(item.first) << "=" << cpr::util::urlEncode(item.second)
                   << "; ";
        }
    }
    return stream.str();
}

std::string& Cookies::operator[](const std::string& key) {
    return map_[key];
}

} // namespace cpr
