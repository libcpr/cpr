#ifndef CPR_INTERCEPTOR_H
#define CPR_INTERCEPTOR_H

#include "cpr/response.h"
#include "cpr/session.h"

namespace cpr {

class Interceptor {
  public:
    class Chain {
      public:
        Chain(Session* session) : session_{session} {}

        Session* session() {
            return session_;
        }

        Response proceed() {
            return session_->proceed();
        }

      private:
        Session* session_;
    };

    virtual ~Interceptor() = default;
    virtual Response intercept(Chain chain) = 0;
};

} // namespace cpr


#endif