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

std::string& Cookies::at(const std::string& key) {
    return map_.at(key);
}

const std::string& Cookies::at(const std::string& key) const {
    return map_.at(key);
}

auto Cookies::begin() -> iterator {
    return map_.begin();
}

auto Cookies::end() -> iterator {
    return map_.end();
}

auto Cookies::begin() const -> const_iterator {
    return map_.begin();
}

auto Cookies::end() const -> const_iterator {
    return map_.end();
}

auto Cookies::cbegin() const -> const_iterator {
    return map_.cbegin();
}

auto Cookies::cend() const -> const_iterator {
    return map_.cend();
}

auto Cookies::rbegin() -> reverse_iterator {
    return map_.rbegin();
}

auto Cookies::rend() -> reverse_iterator {
    return map_.rend();
}

auto Cookies::rbegin() const -> const_reverse_iterator {
    return map_.rbegin();
}

auto Cookies::rend() const -> const_reverse_iterator {
    return map_.rend();
}

auto Cookies::crbegin() const -> const_reverse_iterator {
    return map_.crbegin();
}

auto Cookies::crend() const -> const_reverse_iterator {
    return map_.crend();
}

} // namespace cpr
