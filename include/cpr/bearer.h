#ifndef CPR_BEARER_H
#define CPR_BEARER_H

#include <string>

#include <utility>

namespace cpr {

class Bearer {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Bearer(const std::string& token) : token_string_{token} {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Bearer(std::string&& token) : token_string_{std::move(token)} {}
    Bearer(const Bearer& other) = default;
    Bearer(Bearer&& old) noexcept = default;
    virtual ~Bearer() noexcept = default;

    Bearer& operator=(Bearer&& old) noexcept = default;
    Bearer& operator=(const Bearer& other) noexcept = default;

    virtual const char* GetToken() const noexcept;

  protected:
    std::string token_string_;
};

} // namespace cpr

#endif
