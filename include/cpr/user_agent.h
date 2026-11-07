#ifndef CPR_USER_AGENT_H
#define CPR_USER_AGENT_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <initializer_list>
#include <string>
#endif

#include "cpr/cprtypes.h"

namespace cpr {
EXPORT_CPR class UserAgent : public StringHolder<UserAgent> {
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
