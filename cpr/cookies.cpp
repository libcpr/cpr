#include "cookies.h"

#include "util.h"

namespace cpr {

Cookies::Cookies(const std::initializer_list<std::pair<std::string, std::string>>& pairs) {
    for (auto& pair : pairs) {
        map_[pair.first] = pair.second;
    }
}

std::string Cookies::GetEncoded() const {
    std::stringstream stream;
    for (auto& item : map_) {
        stream << cpr::util::urlEncode(item.first) << "=" << cpr::util::urlEncode(item.second) << "; ";
    }
    return stream.str();
}

std::string& Cookies::operator[](const std::string& key) {
    return map_[key];
}

}
