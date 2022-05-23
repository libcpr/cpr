#ifndef CPR_INTERCEPTOR_H
#define CPR_INTERCEPTOR_H

#include "cpr/response.h"
#include "cpr/session.h"

namespace cpr {

class Interceptor {
  public:
    enum class ProceedHttpMethod { DELETE, DOWNLOAD_CALLBACK, DOWNLOAD_FILE, GET, HEAD, OPTIONS, PATCH, POST, PUT };

    virtual ~Interceptor() = default;
    virtual Response intercept(Session& session) = 0;

  protected:
    static Response proceed(Session& session);

    static Response proceed(Session& session, ProceedHttpMethod httpMethod);

    template <typename T>
    static Response proceed(Session& session, ProceedHttpMethod httpMethod, T&& arg);
};

} // namespace cpr


#endif