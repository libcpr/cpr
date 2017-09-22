#ifndef CPR_CPR_TYPES_H
#define CPR_CPR_TYPES_H

#include <map>
#include <string>

namespace cpr {

struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const noexcept;
};

using Header = std::map<std::string, std::string, CaseInsensitiveCompare>;
using Url = std::string;
using ProtocolVersion = enum class HTTP { v1x, v2 };

} // namespace cpr

#endif
