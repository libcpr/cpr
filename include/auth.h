#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>
#include <utility>

namespace cpr {

    class Authentication {
      public:
        template<typename UserType, typename PassType>
        Authentication(UserType&& username, PassType&& password)
            : username_{std::forward<decltype(username)>(username)},
              password_{std::forward<decltype(password)>(password)},
              auth_string_{username_ + ":" + password_} {}
        Authentication(const char* username, const char* password)
            : Authentication{std::string{username}, std::string{password}} {}

        const char* GetAuthString() const;

      private:
        std::string username_;
        std::string password_;
        std::string auth_string_;
    };

}

#endif