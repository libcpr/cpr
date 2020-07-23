#ifndef CPR_NTLM_H
#define CPR_NTLM_H

#include "auth.h"
#include <utility>

namespace cpr {

class NTLM : public Authentication {
  public:
    using Authentication::Authentication;
};

} // namespace cpr

#endif
