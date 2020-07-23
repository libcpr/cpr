#ifndef CPR_DIGEST_H
#define CPR_DIGEST_H

#include "cpr/auth.h"

namespace cpr {

class Digest : public Authentication {
  public:
    using Authentication::Authentication;
};

} // namespace cpr

#endif
