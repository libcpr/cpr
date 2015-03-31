#ifndef _CPR_AUTH_H_
#define _CPR_AUTH_H_

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
