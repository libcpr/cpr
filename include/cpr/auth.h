#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>

#include <utility>

namespace cpr {

class Authentication {
  public:
    Authentication(const std::string& username, const std::string& password)
            : auth_string_{username + ":" + password} {}
    Authentication(std::string&& username, std::string&& password)
            : auth_string_{std::move(username) + ":" + std::move(password)} {}
    virtual ~Authentication() = default;

    virtual const char* GetAuthString() const noexcept;

  protected:
    std::string auth_string_;
};

} // namespace cpr

#endif
