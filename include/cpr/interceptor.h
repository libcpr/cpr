#ifndef CPR_INTERCEPTOR_H
#define CPR_INTERCEPTOR_H

#include "cpr/response.h"
#include "cpr/session.h"

namespace cpr {

class Interceptor {
  public:
    virtual ~Interceptor() = default;
    virtual Response intercept(Session& session) = 0;

  protected:
    Response proceed(Session& session) {
        return session.proceed();
    }
};

} // namespace cpr


#endif