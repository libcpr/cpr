#ifndef CPR_INTERCEPTOR_H
#define CPR_INTERCEPTOR_H

#include "cpr/response.h"
#include "cpr/session.h"

namespace cpr {

class Interceptor {
  public:
    enum class ProceedHttpMethod {
        GET = 0,
        POST = 1,
        PUT = 2,
        DELETE = 3,
        PATCH = 4,
        HEAD = 5,
        OPTIONS = 6,
        DOWNLOAD_CALLBACK = 7,
        DOWNLOAD_FILE = 8,
    };

    virtual ~Interceptor() = default;
    virtual Response intercept(Session& session) = 0;

  protected:
    static Response proceed(Session& session);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod, std::ofstream& file);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod, const WriteCallback& write);
};

} // namespace cpr


#endif