#ifndef CPR_DOH_URL_H
#define CPR_DOH_URL_H

#include <initializer_list>
#include <string>

#include "cpr/cprtypes.h"

namespace cpr {

class DohUrl : public StringHolder<DohUrl> {
  public:
    DohUrl() = default;
    DohUrl(std::string url) : StringHolder<DohUrl>(std::move(url)) {}
    DohUrl(std::string_view url) : StringHolder<DohUrl>(url) {}
    DohUrl(const char* url) : StringHolder<DohUrl>(url) {}
    DohUrl(const char* str, size_t len) : StringHolder<DohUrl>(str, len) {}
    DohUrl(const std::initializer_list<std::string> args) : StringHolder<DohUrl>(args) {}
    DohUrl(const DohUrl& other) = default;
    DohUrl(DohUrl&& old) noexcept = default;
    ~DohUrl() override = default;

    DohUrl& operator=(DohUrl&& old) noexcept = default;
    DohUrl& operator=(const DohUrl& other) = default;
};

} // namespace cpr

#endif
