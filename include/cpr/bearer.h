#ifndef CPR_BEARER_H
#define CPR_BEARER_H

#include <string>

#include <utility>

namespace cpr {

class Bearer {
  public:
    Bearer(const std::string& token)
            : token_string_{token} {}
    Bearer(std::string&& token)
            : token_string_{std::move(token)} {}
    virtual ~Bearer() = default;

    virtual const char* GetToken() const noexcept;

  protected:
    std::string token_string_;
};

} // namespace cpr

#endif
