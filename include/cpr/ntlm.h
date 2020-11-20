#ifndef CPR_NTLM_H
#define CPR_NTLM_H

#include "cpr/auth.h"

namespace cpr {
class NTLM : public Authentication {
  public:
    NTLM(const std::string& username, const std::string& password)
            : Authentication{username, password} {}
};

} // namespace cpr

#endif
