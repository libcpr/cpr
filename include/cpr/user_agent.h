#ifndef CPR_USER_AGENT_H
#define CPR_USER_AGENT_H

#include <initializer_list>
#include <string>

#include "cpr/cprtypes.h"

namespace cpr {
class UserAgent : public StringHolder<UserAgent> {
  public:
    UserAgent() = default;
    UserAgent(std::string useragent) : StringHolder<UserAgent>(std::move(useragent)) {}
    UserAgent(std::string_view useragent) : StringHolder<UserAgent>(useragent) {}
    UserAgent(const char* useragent) : StringHolder<UserAgent>(useragent) {}
    UserAgent(const char* str, size_t len) : StringHolder<UserAgent>(str, len) {}
    UserAgent(const std::initializer_list<std::string> args) : StringHolder<UserAgent>(args) {}
    UserAgent(const UserAgent& other) = default;
    UserAgent(UserAgent&& old) noexcept = default;
    ~UserAgent() override = default;

    UserAgent& operator=(UserAgent&& old) noexcept = default;
    UserAgent& operator=(const UserAgent& other) = default;
};

} // namespace cpr

#endif
