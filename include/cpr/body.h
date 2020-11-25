#ifndef CPR_BODY_H
#define CPR_BODY_H

#include <initializer_list>
#include <string>

#include "cpr/cprtypes.h"

namespace cpr {

class Body : public StringHolder<Body> {
  public:
    Body() : StringHolder<Body>() {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Body(const std::string& body) : StringHolder<Body>(body) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Body(std::string&& body) : StringHolder<Body>(std::move(body)) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Body(const char* body) : StringHolder<Body>(body) {}
    Body(const char* str, size_t len) : StringHolder<Body>(str, len) {}
    Body(const std::initializer_list<std::string> args) : StringHolder<Body>(args) {}
    Body(const Body& other) = default;
    Body(Body&& old) noexcept = default;
    ~Body() override = default;

    Body& operator=(Body&& old) noexcept = default;
    Body& operator=(const Body& other) = default;
};

} // namespace cpr

#endif
