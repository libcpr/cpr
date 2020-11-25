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
    Authentication(const Authentication& other) = default;
    Authentication(Authentication&& old) noexcept = default;
    virtual ~Authentication() noexcept = default;

    Authentication& operator=(Authentication&& old) noexcept = default;
    Authentication& operator=(const Authentication& other) = default;

    virtual const char* GetAuthString() const noexcept;

  protected:
    std::string auth_string_;
};

} // namespace cpr

#endif
