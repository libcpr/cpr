#ifndef CPR_USERAGENT_H
#define CPR_USERAGENT_H

#include <initializer_list>
#include <string>

#include "cpr/cprtypes.h"

namespace cpr {
class UserAgent : public StringHolder<UserAgent> {
  public:
    UserAgent() : StringHolder<UserAgent>() {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    UserAgent(const std::string& useragent) : StringHolder<UserAgent>(useragent) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    UserAgent(std::string&& useragent) : StringHolder<UserAgent>(std::move(useragent)) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
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
