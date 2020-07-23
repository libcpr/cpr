#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>

#include <utility>

namespace cpr {

class Authentication {
  public:
    Authentication(const std::string& username, const std::string& password)
            : auth_string_{username + ":" + password} {}
    virtual ~Authentication() = default;

    const char* GetAuthString() const noexcept;

  private:
    std::string auth_string_;
};

} // namespace cpr

#endif
