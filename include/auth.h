#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>


class Authentication {
  public:
    Authentication(std::string username, std::string password) :
        username_{username}, password_{password} {};

    const char* GetAuthString() const;

  private:
    std::string username_;
    std::string password_;
};

#endif
