#ifndef CPR_PROXYAUTH_H
#define CPR_PROXYAUTH_H

#include "cpr/auth.h"

namespace cpr {
class ProxyAuth : public Authentication {
  public:
    ProxyAuth(const std::string& username, const std::string& password) : Authentication{username, password} {}
};

} // namespace cpr

#endif