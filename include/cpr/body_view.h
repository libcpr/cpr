#ifndef CPR_BODY_VIEW_H
#define CPR_BODY_VIEW_H

#include <string_view>

#include "cpr/buffer.h"

namespace cpr {

class BodyView final {
  public:
    BodyView() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    BodyView(std::string_view body) : m_body(body) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    BodyView(const char* body) : m_body(body) {}
    BodyView(const char* str, size_t len) : m_body(str, len) {}
    // NOLINTNEXTLINE(google-explicit-constructor, cppcoreguidelines-pro-type-reinterpret-cast)
    BodyView(const Buffer& buffer) : m_body(reinterpret_cast<const char*>(buffer.data), static_cast<size_t>(buffer.datalen)) {}

    BodyView(const BodyView& other) = default;
    BodyView(BodyView&& old) noexcept = default;
    ~BodyView() = default;

    BodyView& operator=(BodyView&& old) noexcept = default;
    BodyView& operator=(const BodyView& other) = default;

    [[nodiscard]] std::string_view str() const { return m_body; }

    private:
      std::string_view m_body;
};

} // namespace cpr

#endif
