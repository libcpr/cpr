#ifndef CPR_NTLM_H
#define CPR_NTLM_H

#include "auth.h"
#include "defines.h"

namespace cpr {

class NTLM : public Authentication {
  public:
    template <typename UserType, typename PassType>
    NTLM(UserType&& username, PassType&& password)
            : Authentication{CPR_FWD(username), CPR_FWD(password)} {}

    const char* GetAuthString() const noexcept;
};

} // namespace cpr

#endif
