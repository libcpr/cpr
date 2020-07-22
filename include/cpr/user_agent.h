#ifndef CPR_USERAGENT_H
#define CPR_USERAGENT_H

#include <initializer_list>
#include <string>

#include "cpr/cprtypes.h"

namespace cpr {
class UserAgent : public StringHolder<UserAgent> {
  public:
    UserAgent() : StringHolder<UserAgent>() {}
    UserAgent(const std::string& useragent) : StringHolder<UserAgent>(useragent) {}
    UserAgent(std::string&& useragent) : StringHolder<UserAgent>(std::move(useragent)) {}
    UserAgent(const char* useragent) : StringHolder<UserAgent>(useragent) {}
    UserAgent(const char* str, size_t len) : StringHolder<UserAgent>(str, len) {}
    UserAgent(const std::initializer_list<std::string> args) : StringHolder<UserAgent>(args) {}
    ~UserAgent() override = default;
};

} // namespace cpr

#endif
