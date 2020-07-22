#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>

#include <utility>

namespace cpr {

class Authentication {
  public:
    Authentication(std::string&& username, std::string&& password)
            : username_(std::move(username)),
              password_(std::move(password)), auth_string_{this->username_ + ":" +
                                                           this->password_} {}
    virtual ~Authentication() = default;

    const char* GetAuthString() const noexcept;

  private:
    const std::string username_;
    const std::string password_;
    const std::string auth_string_;
};

} // namespace cpr

#endif
