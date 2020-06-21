#ifndef CPR_NTLM_H
#define CPR_NTLM_H

#include "auth.h"
#include <utility>

namespace cpr {

class NTLM : public Authentication {
  public:
    template <typename UserType, typename PassType>
    NTLM(UserType&& username, PassType&& password)
            : Authentication{std::move(username), std::move(password)} {}

    const char* GetAuthString() const noexcept;
};

} // namespace cpr

#endif
