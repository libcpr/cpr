#ifndef CPR_INTERCEPTOR_H
#define CPR_INTERCEPTOR_H

#include "cpr/response.h"
#include "cpr/session.h"

namespace cpr {

class Interceptor {
  public:
    enum class ProceedHttpMethod {
        GET_REQUEST = 0,
        POST_REQUEST = 1,
        PUT_REQUEST = 2,
        DELETE_REQUEST = 3,
        PATCH_REQUEST = 4,
        HEAD_REQUEST = 5,
        OPTIONS_REQUEST = 6,
        DOWNLOAD_CALLBACK_REQUEST = 7,
        DOWNLOAD_FILE_REQUEST = 8,
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