#include "cpr/cookies.h"

#include "cpr/util.h"

namespace cpr {

Cookies::Cookies(const std::initializer_list<std::pair<const std::string, std::string>>& pairs)
        : map_{pairs} {}

std::string Cookies::GetEncoded() const {
    std::stringstream stream;
    for (const auto& item : map_) {
        stream << cpr::util::urlEncode(item.first) << "=";
        // special case version 1 cookies, which can be distinguished by
        // beginning and trailing quotes
        if (!item.second.empty() && item.second.front() == '"' && item.second.back() == '"') {
            stream << item.second;
        } else {
            stream << cpr::util::urlEncode(item.second);
        }
        stream << "; ";
    }
    return stream.str();
}

std::string& Cookies::operator[](const std::string& key) {
    return map_[key];
}

Cookies::iterator Cookies::begin() {
    return map_.begin();
}

Cookies::iterator Cookies::end() {
    return map_.end();
}

Cookies::const_iterator Cookies::begin() const {
    return map_.begin();
}

Cookies::const_iterator Cookies::end() const {
    return map_.end();
}

Cookies::const_iterator Cookies::cbegin() const {
    return map_.cbegin();
}

Cookies::const_iterator Cookies::cend() const {
    return map_.cend();
}

} // namespace cpr
