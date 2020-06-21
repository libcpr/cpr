#ifndef CPR_BODY_H
#define CPR_BODY_H

#include <initializer_list>
#include <string>

#include "cpr/cprtypes.h"

namespace cpr {

class Body : public StringHolder<Body> {
  public:
    Body() : StringHolder<Body>() {}
    explicit Body(const std::string& body) : StringHolder<Body>(body) {}
    explicit Body(const std::string&& body) : StringHolder<Body>(std::move(body)) {}
    explicit Body(const char* body) : StringHolder<Body>(body) {}
    Body(const char* str, size_t len) : StringHolder<Body>(str, len) {}
    Body(const std::initializer_list<std::string> args) : StringHolder<Body>(args) {}
    ~Body() override = default;
};

} // namespace cpr

#endif
