#ifndef CPR_INTERFACE_H
#define CPR_INTERFACE_H

#include <initializer_list>
#include <string>

#include "cpr/cprtypes.h"

namespace cpr {

class Interface : public StringHolder<Interface> {
  public:
    Interface() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Interface(std::string iface) : StringHolder<Interface>(std::move(iface)) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Interface(std::string_view iface) : StringHolder<Interface>(iface) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Interface(const char* iface) : StringHolder<Interface>(iface) {}
    Interface(const char* str, size_t len) : StringHolder<Interface>(str, len) {}
    Interface(const std::initializer_list<std::string> args) : StringHolder<Interface>(args) {}
    Interface(const Interface& other) = default;
    Interface(Interface&& old) noexcept = default;
    ~Interface() override = default;

    Interface& operator=(Interface&& old) noexcept = default;
    Interface& operator=(const Interface& other) = default;
};

} // namespace cpr

#endif
